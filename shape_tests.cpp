#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include "shape.h"
#include "poly_utils.h"

using namespace std;
void makeOracleFile( char *filename, ostream &fout );
string gen_random( const int len );
void makeTestFile( int totalRegions, int totalTests, int baselineTests, char* outfile, char* oracleOutFile );
Shape* toSVG( char *infile, char *outfile );


/*
 * Generates a random string of letters
 * and digits.  The length is given by len.
*/
std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

bool overlaps( Shape *s, set<Shape*>& regions ) {  
  double* shape_box = s->bounding_box();
  
  for( set<Shape*>::iterator it = regions.begin(); it != regions.end(); it++ ) { // for every existing shape
    double* existing_box = (*it)->bounding_box();
    if( boxes_intersect( shape_box, existing_box ) ) {
      delete [] shape_box;
      delete [] existing_box;
      return true;
    }
    delete existing_box;
  }
  delete [] shape_box;
  return false;
}

/*
 * Creates a random set of Shapes
 * using the default constructor.
 * The default constructor ensures
 * that each point is on the page.
 */
set<Shape*> makeTestSet( int totalRegions ) {
  set<Shape*> regions;
  cout << "STARTING" << endl;
  while( regions.size() < totalRegions ) {
    Shape *s = new Shape( gen_random(2), true );
    if( !overlaps( s, regions ) ) {
      regions.insert( s );
    } else {
      delete s;
    }
  }

  cout << "ENDING" << endl;
  return regions;
}

/*
This function makes a "test" file named by "outfile".  This file has the following format:
First Line: contains a single integer N denoting the total number of shapes.
Next N Lines: each line contains an id-string followed by six doubles denoting the (x1,y1), (x2,y2), (x3, y3) points of a leaf shape.
Next Line: contains a single integer M denoting the total number of hit queries.
Next M Lines: each line contains two doubles denoting the (x,y) of a hit query.

This function also makes an "oracle" file that gives the correct answers for each hit query in the test file.
This file contains M lines describing the results of each hit-query contained in the above outfile.
*/
void makeTestFile( int totalRegions, int totalTests, int baselineTests, char* outfile, char* oracleOutFile ) {
  fstream fout;
  fout.open( outfile, fstream::out );
  
  set<Shape*> regions = makeTestSet( totalRegions );
  set<Shape*> baselines;

  for( set<Shape*>::iterator it = regions.begin(); baselines.size() < baselineTests; it++ ) {
    baselines.insert( *it );
  }
  
  ///////////////////////////////////////////////////////////
  // OUTPUT THE SHAPES; STARTING WITH THE NUMBER OF SHAPES
  ///////////////////////////////////////////////////////////
  fout << totalRegions << endl;
  for( auto it : regions ) {
    Shape *shape = it;
    list<Point*> *hull = shape->getPoints();
    
    fout << shape->getId() << " "; // print the id
    for( list<Point*>::iterator it1 = hull->begin(); it1 != hull->end(); it1++ ) {
      Point* pt = *it1;
      fout << pt->x << " " << pt-> y << " "; // print each point
    }
    fout << endl;    
    delete hull;
  }
  
  ////////////////////////////////////////////////////////////////////////
  // OUTPUT THE HIT QUERIES; STARTING WITH THE TOTAL NUMBER OF HIT QUERIES
  ////////////////////////////////////////////////////////////////////////
  fout << totalTests + 1 << endl;
  // RANDOM LOCATION QUERIES
  for( int i = 0; i < totalTests-baselineTests; i++  ) {
    double x = rand() % Shape::PAGE_DIMENSION;
    double y = rand() % Shape::PAGE_DIMENSION;    
    fout << x  << " " << y << endl;  
  }

  // SHAPE CENTROID QUERIES
  for( auto it : baselines ) { // these leaves are in the input file
    Point* p = it->getCentroid();
    fout << p->x << " " << p->y << endl; // look for it's center
    free(p);
  }

  // FOR SURE NULL
  fout << -1  << " " << -1 << endl;  // ADD A TEST FOR NULL
  fout.close();  

  // CREATE THE ORACLE OUTPUT FILE
  fstream oracle;
  oracle.open( oracleOutFile, fstream::out );
  makeOracleFile( outfile, oracle );

  oracle.close();
}

/*
 * Accepts the name of a test-file and an output stream.
 * Outputs the results of hit-testing against the reduced
 * circles given in the test-file
 */
void makeOracleFile( char *filename, ostream &fout ) {
  int size;
  set<Shape*> regions;
  fstream fin;
  fin.open( filename, fstream::in );

  // READ THE REGIONS INTO A SET
  fin >> size;
  for( int i = 0; i < size; i++ ) {
    string id;
    fin >> id;
    Shape* shape = new Shape(id, false);
    
    for( int p = 0; p < 3; p++ ) { 
      double x, y;
      fin >> x >> y;
      (*shape) << (new Point( x, y ));
    }

    regions.insert( shape );
  }
  // GENERATE THE HIT-TESTING TREE
  Shape *root = reduce( regions ); 
  // RUN HIT TESTS AGAINST THE TREE
  fin >> size;
  for( int i = 0; i < size; i++ ) {
    double  x, y;
    fin >> x >> y;

    Shape *hit = (*root)(x,y);

    if( hit != NULL ) {
      fout << "root[" << root->getId() << "](" << x << ", " << y << ") => " << hit->getId() << endl;
    } else {
      fout << "root(" << x << ", " << y << ") => NULL" << endl;
    }
  }

  fin.close();
}

/*
 * Accepts the name of a test-file and the name of an SVG file.
 * Generates an SVG file that describes the reducing of the circles in the test-file
 */
Shape* toSVG( char *infile, char *outfile ) {
  set<Shape*> regions;
  fstream fin( infile, fstream::in );
  fstream fout( outfile, fstream::out );

  int SIZE;
  fin >> SIZE;
  for( int i = 0; i < SIZE; i++ ) {
    string id;
    fin >> id;
    Shape* shape = new Shape(id, false);
    for( int p = 0; p < 3; p++ ) { 
      double x, y;
      fin >> x >> y;
      (*shape) << (new Point( x, y ));

      regions.insert( shape );
    }
  }
  
  Shape *root = reduce( regions );
  int min_x = 0;
  int min_y = 0;
  int size =  200;
  
  fout << "<svg viewBox=\"" << min_x << " " << min_y << " " << size << " " << size << " " << "\" xmlns=\"http://www.w3.org/2000/svg\">" << endl;
  fout / *root << endl;
  fout << "</svg>";

  fout.close();
  fin.close();
  return root;
}

void usage() {
  cout << "Usage:" << endl;
  cout << "circular TOTAL_CIRCLES TOTAL_TESTS BASELINE_TESTS OUTFILE ORACLEFILE" << endl;
  cout << "circular INFILE OUTFILE" << endl;
  cout << "circular INFILE" << endl;

  cout << "The first option is used to generate a test-file.  The file contains N circles and M hit-queries.  The circles are randomly placed and sized as are some of the hit queries." << endl;
  cout << "The second option converts the circles of a test-file into an SVG file." << endl;
  cout << "The final option first a) calls reduce and then b) executes hit-queries against the root circle" << endl;
}

int main( int argc, char** argv ) {
  srand (time(NULL));
  
  if( argc == 6 ) {
    int totalCircles = atoi( argv[1] );
    int totalTests = atoi( argv[2] );
    int baselineTests = atoi( argv[3] );

    if( totalTests < baselineTests || totalCircles < baselineTests) {
      cout << "totalTests must be greater-than-or-equalt-to baselineTests" << endl;
      exit(1);
    }

    makeTestFile( totalCircles, totalTests, baselineTests, argv[4], argv[5] );

  } else if( argc == 3 ) {
      toSVG( argv[1], argv[2] );
  } else if( argc == 2 ) {
      makeOracleFile( argv[1], cout );
  } else {
      usage();
  }

}
