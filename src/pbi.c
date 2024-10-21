
#include "pbi.h"
#include <raymath.h>
#include <stdlib.h>
#include <math.h>


float percentage = 30;

PBI pbi;


int Partition(float *distances, int *permutation, int low, int high){
  float pivot = distances[high];
  int i = low - 1;

  for (int j = low; j < high; j++){
    if (distances[j] < pivot) {
      i++;
      float temp = distances[i];
      distances[i] = distances[j];
      distances[j] = temp;

      int temp2 = permutation[i];
      permutation[i] = permutation[j];
      permutation[j] = temp2;
    }
  }

  float temp = distances[i + 1];
  distances[i + 1] = distances[high];
  distances[high] = temp;

  int temp2 = permutation[i + 1];
  permutation[i + 1] = permutation[high];
  permutation[high] = temp2;

  return i + 1;
}

void QuickSort(float *distances, int *permutation, int low, int high) {
  if (low < high) {
    int pivot = Partition(distances, permutation, low, high);
    QuickSort(distances, permutation, low, pivot - 1);
    QuickSort(distances, permutation, pivot + 1, high);
  }
}


void SortPermutation(Object *object){
  int i, j;
  float *distances = malloc(sizeof(float) * pbi.nPermutants);

  int x1 = object->position.x;
  int y1 = object->position.y;

  for (i = 0; i < pbi.nPermutants; i++) {
    object->permutation[i] = i;
    int x2 = pbi.permutants[i]->position.x;
    int y2 = pbi.permutants[i]->position.y;
    distances[i] = sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
  }

  /*for (i = 0; i < pbi.nPermutants - 1; i++) {*/
  /*  for (j = i + 1; j < pbi.nPermutants; j++) {*/
  /*    if (distances[i] > distances[j]) {*/
  /*      break;*/
  /*    }*/
  /*  }*/
  /**/
  /*  if (j < pbi.nPermutants) {*/
  /*    float tempd = distances[i];*/
  /*    distances[i] = distances[j];*/
  /*    distances[j] = tempd;*/
  /**/
  /*    int temp = object->permutation[i];*/
  /*    object->permutation[i] = object->permutation[j];*/
  /*    object->permutation[j] = temp;*/
  /**/
  /*  }*/
  /*}*/

  QuickSort(distances, object->permutation, 0, pbi.nPermutants - 1);


  free(distances);
}


void InitPBI() {
  pbi.nPermutants = 0;
  pbi.size = 0;
  pbi.candidatesSize = 0;

  pbi.nPermutantsCapacity = 200;
  pbi.capacity = 200;

  pbi.queryAdded = false;

  pbi.objects = malloc(sizeof(Object *) * pbi.capacity);
  pbi.permutants = malloc(sizeof(Object *) * pbi.nPermutantsCapacity);
  pbi.query = malloc(sizeof(Object));
  pbi.candidates = malloc(sizeof(Object *) * pbi.capacity);
}

void FreePBI() {

  for (int i = 0; i < pbi.size; i++) {
    free(pbi.objects[i]->permutation);
    free(pbi.objects[i]);
  }

  free(pbi.permutants);
  free(pbi.objects);
}

void AddObject(Vector2 position) {

  Object *object = malloc(sizeof(Object));
  object->id = pbi.size;
  object->position = position;
  object->type = OBJECT;
  object->permutation = malloc(sizeof(int) * MAX_PERMUTANTS);

  /*if (pbi.size >= pbi.capacity) {*/
  /*  pbi.capacity *= 2;*/
  /*  pbi.objects = realloc(pbi.objects, sizeof(Object *) * pbi.capacity);*/
  /*}*/

  pbi.objects[pbi.size] = object;

  pbi.size++;
}

void AddPermutant(Vector2 position) {

  Object *object = malloc(sizeof(Object));
  object->id = pbi.size;
  object->permutantIndex = pbi.nPermutants;
  object->position = position;
  object->type = PERMUTANT;
  object->permutation = malloc(sizeof(int) * MAX_PERMUTANTS);

  /*if (pbi.size >= pbi.capacity) {*/
  /*  pbi.capacity *= 2;*/
  /*  pbi.objects = realloc(pbi.objects, sizeof(Object *) * pbi.capacity);*/
  /*}*/
  /*if (pbi.nPermutants >= pbi.nPermutantsCapacity) {*/
  /*  pbi.nPermutantsCapacity *= 2;*/
  /*  pbi.permutants = realloc(pbi.permutants, sizeof(Object *) * pbi.nPermutantsCapacity);*/
  /*}*/

  pbi.objects[pbi.size] = object;
  pbi.permutants[pbi.nPermutants] = object;


  pbi.nPermutants++;
  pbi.size++;


}

void AddQuery(Vector2 position){

  pbi.query->id = pbi.size;
  pbi.query->position = position;
  pbi.query->type = QUERY;
  pbi.query->permutation = malloc(sizeof(int) * MAX_PERMUTANTS);
  pbi.queryAdded = true;
}



int *InversePermutation(int *p, int size){
  int i;
  int *inverse = malloc(sizeof(int) * size);

  for (i = 0; i < size; i++){
    inverse[p[i]] = i;
  }

  return inverse;
}

int SpearmanRho(int *a, int *b, int size){
  int sum = 0;
  int i;

  int *inverse = InversePermutation(a, size);
  for (i = 0; i < size; i++) {
    sum += abs(i - inverse[b[i]]);
  }

  free(inverse);
  return sum;
}

Object * searchObject(Vector2 position) {
  Object *object = NULL;

  for (int i = 0; i < pbi.size; i++) {
    if (Vector2Distance(pbi.objects[i]->position, position) < 20) {
      object = pbi.objects[i];      
    }
  }

  if (!object && Vector2Distance(pbi.query->position, position) < 20) {
    object = pbi.query;
  }

  return object;
}


int comparate(const void *a, const void *b) {
  Object *_a = *(Object**)a;
  Object *_b = *(Object**)b;
  return _a->spearmanRhoToQuery - _b->spearmanRhoToQuery;
}


void updatePBI() {

  for (int i = 0; i < pbi.size; i++) {
    SortPermutation(pbi.objects[i]);
  }

  if (pbi.queryAdded) {
    SortPermutation(pbi.query);
    for (int i = 0; i < pbi.size; i++) {
      pbi.objects[i]->spearmanRhoToQuery = SpearmanRho(
        pbi.query->permutation, pbi.objects[i]->permutation, pbi.nPermutants);
    }
    qsort(pbi.objects, pbi.size, sizeof(Object *), comparate);

  }


}


void RangeSearch(float r) {
  pbi.candidatesSize = 0;

  int x = pbi.size * (percentage / 100);
  int x2 = pbi.query->position.x;
  int y2 = pbi.query->position.y;

  for (int i = 0; i < x; i++) {
    int x1 = pbi.objects[i]->position.x;
    int y1 = pbi.objects[i]->position.y;
    float d = sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    if (d <= r) {
      pbi.candidates[i] = pbi.objects[i];      
      pbi.candidatesSize++;
    }
  }
}

typedef struct {
  Object *object;
  float distance;
} Candidate;

int compareCandidate (const void *a, const void *b) {
  Candidate *_a = (Candidate*)a;
  Candidate *_b = (Candidate*)b;
  return _a->distance - _b->distance;
}

void KNNSearch(int k) {
  pbi.candidatesSize = 0;
  int x = pbi.size * (percentage / 100);

  Candidate *candidates = malloc(sizeof(Candidate) * x);

  for (int i = 0; i < x; i++) {
    candidates[i].object = pbi.objects[i];
    candidates[i].distance = Vector2Distance(pbi.objects[i]->position, pbi.query->position);
  }

  qsort(candidates, x, sizeof(Candidate), compareCandidate);
  for (int i = 0; i < x && i < k; i++) {
    pbi.candidates[i] = candidates[i].object;
    pbi.candidatesSize++;
  }


  free(candidates);
}

