#include <raylib.h>

#define MAX_PERMUTANTS 200

typedef enum {
  OBJECT,
  PERMUTANT,
  QUERY
}Type ;

typedef struct {
  int id;
  Vector2 position;
  Type type;
  int permutantIndex;
  int *permutation;
  int spearmanRhoToQuery;
} Object;


typedef struct {
  int nPermutants;
  int nPermutantsCapacity;
  Object **permutants;

  int size;
  int capacity;
  Object **objects;

  int candidatesSize;
  Object **candidates;

  int nFicticious;
  int nFicticiousCapacity;
  float *ficticiousDistances;

  bool queryAdded;
  Object *query;
} PBI;

extern float percentage;

extern PBI pbi;
extern float *distances;
extern float *sortedDistances;


void InitPBI();
void FreePBI();
void AddObject(Vector2 position);
void AddPermutant(Vector2 position);
void AddQuery(Vector2 position);
void CalculatePermutation(Object *object);
int *InversePermutation(int *p, int size); 
int SpearmanRho(int *a, int *b, int size);
void updatePBI();
Object * searchObject(Vector2 position);

void RangeSearch(float r);
void KNNSearch(int k);

