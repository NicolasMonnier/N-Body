#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

#include <json-c/json.h>

#include "toy_nBody.h"

#ifndef SIZE
#define SIZE 3000
#endif




struct Object{
  float x, y, z; 
  float vx, vy, vz;
  float ax, ay, az;
  float mass;
};

// Needed by MPI
int* my_start;
int* my_end;

int x;

struct Object objects[SIZE];
float meanv;

float totalfps = 0.0f; 
float totaltime = 0.0f;
int frames = 0;
float max_fps = 0.0f; 
float min_fps = 0.0f;

void compute()
{
  const float softeningSquared = 0.001215000f*0.001215000f;
  const float G = 6.67259e-11f;
  const float timestep = 0.1f;
  const float damping = 1.0f;   

  int iter = 0;
  float ax=0.0, ay=0.0, az=0.0;
  int i = 0;
  int j = 0;
//#pragma omp parallel private(i)
//{

  #pragma omp parallel for private(j, ax, ay, az) 
  for (i = my_start[0]; i < my_end[0]; i++)// update velocity
    {
      for (j = 0; j < SIZE; j++)
      {
          if (i < j || i > j)
          {
              float distance[3];
              float distanceSqr = 0.0f, distanceInv = 0.0f;
              distance[0] = objects[j].x - objects[i].x;
              distance[1] = objects[j].y - objects[i].y;
              distance[2] = objects[j].z - objects[i].z;
				
              distanceSqr = sqrt(distance[0]*distance[0] + distance[1]*distance[1] + distance[2]*distance[2]) + softeningSquared;
              distanceInv = 1.0f / sqrt(distanceSqr);
              ax += distance[0] * G * objects[j].mass * distanceInv * distanceInv * distanceInv;
              ay += distance[1] * G * objects[j].mass * distanceInv * distanceInv * distanceInv;
              az += distance[2] * G * objects[j].mass * distanceInv * distanceInv * distanceInv;
          }
      }

      objects[i].ax = ax;
      objects[i].ay = ay;
      objects[i].az = az;
      ax = 0.0;
      ay = 0.0;
      az = 0.0;
    }
//} 

  for (i = my_start[0]; i < my_end[0]; ++i)// update position
    {		
      objects[i].vx += objects[i].ax * timestep * damping;
      objects[i].vy += objects[i].ay * timestep * damping;
      objects[i].vz += objects[i].az * timestep * damping;
		
      objects[i].x += objects[i].vx * timestep;
      objects[i].y += objects[i].vy * timestep;
      objects[i].z += objects[i].vz * timestep;

      objects[i].ax = 0.0f;
      objects[i].ay = 0.0f;
      objects[i].az = 0.0f;

      meanv += objects[i].vx + objects[i].vy + objects[i].vz;
    }

}

void update()
{


  struct timeval time1, time2;
  char textBuf[512];
  float frameTime = 0;
  float fps = 0;
  long mtime, seconds, useconds;
  float meanv_tmp;
  int i;

  gettimeofday(&time1, NULL);

  compute();

  gettimeofday(&time2, NULL);
	
  seconds = time2.tv_sec - time1.tv_sec;
  useconds = time2.tv_usec - time1.tv_usec;

  frameTime = seconds + useconds/(1000.0*1000.0); 
  totaltime += frameTime;
  frames++;

  fps = frames / totaltime;

  //only master print intermediate time
  if (frames % 50 == 0)
  {
      printf("%d|t: %.02fs=%.01f FPS\n", frames, totaltime, fps);
  }
}

void read_json_file(char *filename, float *x0, float *y0, float *z0, float *vx0, float *vy0, float *vz0, float *mass0)
{
    FILE *fp;
    //char *buffer = malloc(1024 * sizeof(char));
    char buffer[1024];
    struct json_object *parsed_json;
    struct json_object *x;
    struct json_object *y;
    struct json_object *z;
    struct json_object *vx;
    struct json_object *vy;
    struct json_object *vz;
    struct json_object *mass;

    fp = fopen(filename, "r");
    fread(buffer, 1024, 1, fp);
    fclose(fp);

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "x", &x);
    json_object_object_get_ex(parsed_json, "y", &y);
    json_object_object_get_ex(parsed_json, "z", &z);
    json_object_object_get_ex(parsed_json, "vx", &vx);
    json_object_object_get_ex(parsed_json, "vy", &vy);
    json_object_object_get_ex(parsed_json, "vz", &vz);
    json_object_object_get_ex(parsed_json, "mass", &mass);

    *x0 = json_object_get_double(x);
    *y0 = json_object_get_double(y);
    *z0 = json_object_get_double(z);
    *vx0 = json_object_get_double(vx);
    *vy0 = json_object_get_double(vy);
    *vz0 = json_object_get_double(vz);
    *mass0 = json_object_get_double(mass);

    //free(buffer);
}


static void init_objects(char *filename)
{

  int i = 0;
  float x0, y0, z0, vx0, vy0, vz0, mass0;

  read_json_file(filename, &x0, &y0, &z0, &vx0, &vy0, &vz0, &mass0);


   //srand((unsigned int)time(NULL)); 
  printf("RAND_MAX = %d ", RAND_MAX);
  for (i = 1; i < SIZE; ++i)
    {
      objects[i].x = init_array[i][0];
      objects[i].y = init_array[i][1];
      objects[i].z = init_array[i][2];
      objects[i].vx = init_array[i][3];
      objects[i].vy = init_array[i][4];
      objects[i].vz = init_array[i][5];
      objects[i].ax = 0.0f;
      objects[i].ay = 0.0f;
      objects[i].az = 0.0f;
      objects[i].mass = init_array[i][6];

      //printf("{%f , %f , %f, %f, %f, %f, %f},\n", objects[i].x, objects[i].y, objects[i].z, objects[i].vx, objects[i].vy, objects[i].vz, objects[i].mass);
    }
} 

static struct Object crc(void)
{
  int i;
  struct Object result;

  result.x  = 0.0;
  result.y  = 0.0;
  result.z  = 0.0;
  result.vx = 0.0;
  result.vy = 0.0;
  result.vz = 0.0;
  result.ax = 0.0;
  result.ay = 0.0;
  result.az = 0.0;
  result.mass = 0.0;
  
  for (i = 0; i < SIZE; ++i)
    {
      result.x  += objects[i].x;
      result.y  += objects[i].y;
      result.z  += objects[i].z;
      result.vx += objects[i].vx;
      result.vy += objects[i].vy;
      result.vz += objects[i].vz;
      result.ax += objects[i].ax;
      result.ay += objects[i].ay;
      result.az += objects[i].az;
      result.mass += objects[i].mass;
    }
  return result;
}

int main(int argc, char* argv[])
{
  x = 200;
  int i = 0;


  char *mystring = argv[1];
  
  //master initialize meanv, x and objects 
  init_objects(mystring);


  printf("starting simulation for %d frames...\n", x);

  // use of tables, so that all processes knows the bounds of the others
  my_start = (int *) malloc(sizeof(int));
  my_end   = (int *) malloc(sizeof(int));
  my_start[0] = 0;
  my_end[0] = SIZE;
  
  my_start[1] = my_end[0];
  my_end[1] = my_start[1] + SIZE ;
  
  my_end[0] = SIZE;

  //initialization of meanv
  meanv = 0.0;

  for (i=0; i != x; ++i)
    {
      update();
    }



  //master print the CRC
  printf("simulation finished\n");
  struct Object crcr = crc();     
  //{
  //  printf("crc ERROR =%g %g %g %g %g %g %g\n", crcr.x, crcr.y, crcr.y, crcr.vx, crcr.vy, crcr.vz, meanv);
  //}

  FILE *fpout;
  fpout = fopen("output.csv", "w");
  const char *header ="x y z vx vy vz";
  fprintf(fpout, "%s\n", header);
  fprintf(fpout, "%g %g %g %g %g %g\n", crcr.x, crcr.y, crcr.y, crcr.vx, crcr.vy, crcr.vz);
  fclose(fpout);
  return 0;
}
