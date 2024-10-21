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
  int size;
  int capacity;
  bool queryAdded;
  int candidatesSize;


  Object *query;
  Object **permutants;
  Object **objects;
  Object **candidates;
} PBI;

extern float percentage;

extern PBI pbi;


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

