/*/../bin/ls > /dev/null
 if [ $# -ne 1 ]
 then
     echo "Usage: $0 <number of points to generate>"
     exit
 fi
 filename=$(basename $BASH_SOURCE)
 dirname=$(cd $(dirname $BASH_SOURCE) && pwd)
 if [ "${dirname}" == "$(pwd)" ]
 then
    mkdir -p ${dirname}/bin > /dev/null
    filename="${dirname}/bin/${filename}"
 else
    filename="./${filename}"
 fi
 filename=${filename%.*}
 echo $filename, $dirname
 if [ $0 -nt ${filename} ]
 then
	g++ -o "${filename}" --std=c++11  $BASH_SOURCE -lpthread || exit
 fi
 ("${filename}" $1 && exit) || echo $? && exit
 exit
*/
#include <future>
#include <iostream>
#include <random>
#include <string>
#include <thread>
using namespace std;

// given a point (x, y) where x<1.0, y<1.0
// check if it is within a unit circle.
bool is_point_inside_circle(double x, double y)
{
	return (x*x + y*y) <= 1.0;
}

// given a n number of randomly generated
// points within a square just enclosing
// a unit circle, find number of points
// that lie within the circle.

// Actually, only need to check the upper quarter.
int compute_point_count_inside_circle(int n)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);

    int in_count = 0;

    for (int i = 0; i < n; ++i) {
    	double x = dis(gen);
    	double y = dis(gen);
        if (is_point_inside_circle(x, y)) {
        	in_count++;
        }
    }
	return in_count;
}



int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " <number of points to generate>" << endl;
		return 1;
	}

	int total_number_points = stoi(argv[1]);

	int cpu_count = thread::hardware_concurrency();

	future<int> fut[cpu_count];

	int number_points_per_cpu = total_number_points / cpu_count;

	for (int i=0; i<cpu_count-1; i++)
	{
		fut[i] = async(launch::async,
				      compute_point_count_inside_circle,
					  number_points_per_cpu);
	}

	fut[cpu_count-1] = async(launch::async,
			                      compute_point_count_inside_circle,
								  total_number_points - (cpu_count-1) * number_points_per_cpu);


	int total_number_inside_points = 0;
	for (int i=0; i<cpu_count; i++)
	{
		total_number_inside_points += fut[i].get();
	}
	cout << "PI is " << (double ) 4 * total_number_inside_points / total_number_points << endl;

}
