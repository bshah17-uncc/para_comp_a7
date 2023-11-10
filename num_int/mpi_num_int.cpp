#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

  
int main (int argc, char* argv[]) {
  
  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  int functionid = atoi(argv[1]);
  double a = atof(argv[2]);
  double b = atof(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);

  float (*f)(float, int);
  switch (functionid) {
    case 1: f = f1; break;
    case 2: f = f2; break;
    case 3: f = f3; break;
    case 4: f = f4; break;
    default: std::cerr<<"functionid must be 1, 2, 3 or 4"<<std::endl; return -1;
  }


  MPI_Init(&argc, &argv);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  double temp = (b - a) / n;
  double local_sum = 0.0;

  // Timing the execution
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  if (world_rank == 0) {
      start = std::chrono::high_resolution_clock::now();
  }

  // Divide the task among processes
  int local_n = n / world_size;
  int start = world_rank * local_n;
  int end = start + local_n;
  if (world_rank == world_size - 1) {
    end = n;
  }

  for(int i = start; i < end; i++) {
    float x = a + (i + 0.5) * temp;
    local_sum += f(x, intensity);
  }

  double global_sum = 0.0;
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (world_rank == 0) {
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cerr << elapsed_seconds.count() << std::endl;
    std::cout << global_sum * temp << std::endl;
  }

  MPI_Finalize();  

  return 0;
}
