
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "pbi.h"

const int screenWidth = 1920;
const int screenHeight = 1080;

#define ELEMENT_RADIUS 10



typedef enum {
  CREATE_OBJECT,
  CREATE_PERMUTANT,
  CREATE_QUERY,
  DRAGGING,
} DrawMode;

typedef enum {
  KNN,
  RANGE
} Query;



typedef struct {
  DrawMode drawMode;
  Object *draggedObject;
  Camera2D camera;
  Vector2 mouseWorldPos;
  Query queryType;
  float queryRange;
  int QueryK;
} State;

State state;


void Init(){
  InitPBI();
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "Permutation interactive demo");

  state.drawMode = DRAGGING;
  state.draggedObject = NULL;
  state.camera.target = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f }; 
  state.camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
  state.camera.zoom = 1.0f;
  state.queryType = KNN;
  state.queryRange = 100;
  state.QueryK = 5;

  SetTargetFPS(60);
  GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

}
char *mode[] = {"CREATE OBJECT", "CREATE PERMUTANT", "CREATE QUERY", "DRAGGING"};
char *objectType[] = {"OBJECT", "PERMUTANT", "QUERY"};

void DrawPermutant(Object *object) {
  DrawCircleV(object->position, ELEMENT_RADIUS, GREEN);
}

void DrawQuery(Object *object) {
  DrawRectangleV(
    (Vector2){
      object->position.x - ELEMENT_RADIUS, 
      object->position.y - ELEMENT_RADIUS}, 
    (Vector2){ELEMENT_RADIUS * 2, ELEMENT_RADIUS * 2}, BLUE
  );
}

void DrawObject(Object *object) {
  DrawCircleV(object->position, ELEMENT_RADIUS, RED);
}

void DrawPermutation(Object *object) {
  int i = 0;
  DrawText("{", object->position.x - 8 * (pbi.nPermutants), object->position.y - 30, 8, MAROON);

  for (i = 0; i < pbi.nPermutants; i++) {
    DrawText(
      TextFormat((i < pbi.nPermutants - 1) ? "%d, " : "%d", object->permutation[i]), 
      object->position.x - 8 * (pbi.nPermutants - i - 1) + 4 * i, 
      object->position.y - 30, 
      8, 
      MAROON);
  }
  DrawText("}", object->position.x + 4 * (pbi.nPermutants) + 3, object->position.y - 30, 8, MAROON);
}

void InputHandler() {

  if (IsKeyPressed(KEY_SPACE)) {
    state.drawMode++;

    if (state.drawMode > DRAGGING) {
      state.drawMode = CREATE_OBJECT;
    }
  }

  if (state.drawMode == DRAGGING) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      state.draggedObject = searchObject(state.mouseWorldPos);      
    }
  }

  if (state.draggedObject) {
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      state.draggedObject = NULL;
    }

  }


  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -1.0 / state.camera.zoom);
    state.camera.target = Vector2Add(state.camera.target,delta);
  }


  float wheel = GetMouseWheelMove();

  if (wheel != 0) {
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), state.camera);

    state.camera.offset = GetMousePosition();
    state.camera.target = mouseWorldPos;

    float scaleFactor = 1.0 + (0.25 * fabsf(wheel));
    if (wheel < 0) scaleFactor = 1.0 / scaleFactor; 
    state.camera.zoom = Clamp(state.camera.zoom * scaleFactor, 0.125, 64.0);
  }


  // reset the visualizer

  if (IsKeyPressed(KEY_R)){
    state.camera.zoom = 1.0f;
    state.camera.target = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f }; 
    state.camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };

    state.queryType = KNN;
    state.queryRange = 100;
    state.QueryK = 5;

    FreePBI();
    InitPBI();
  }


}

void DrawElement() {

  switch (state.drawMode) {
    case CREATE_OBJECT:
      DrawCircleV(state.mouseWorldPos, 10, RED);
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AddObject(state.mouseWorldPos);
        updatePBI();

      }
      break;
    case CREATE_PERMUTANT:
      DrawCircleV(state.mouseWorldPos, 10, GREEN);
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AddPermutant(state.mouseWorldPos);
        updatePBI();
      }
      break;
    case CREATE_QUERY:
      DrawRectangleV(
        (Vector2){
          state.mouseWorldPos.x - 10,
          state.mouseWorldPos.y - 10}, 
        (Vector2){20, 20}, BLUE
      );
      if (!pbi.queryAdded &&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AddQuery(state.mouseWorldPos);
        updatePBI();
      }
      break;
    case DRAGGING:
      if (state.draggedObject != NULL && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        state.draggedObject->position = state.mouseWorldPos;
        DrawText(
          TextFormat("%s", objectType[state.draggedObject->type]), 
          state.draggedObject->position.x, 
          state.draggedObject->position.y - 20, 
          10, 
          MAROON);
        // position
        DrawText(
          TextFormat("(%0.2f, %0.2f)", state.draggedObject->position.x, state.draggedObject->position.y), 
          state.draggedObject->position.x, 
          state.draggedObject->position.y + 20, 
          10, 
          MAROON);
        updatePBI();

      }

      break;


  }

  for (int i = 0; i < pbi.size; i++) {
    switch (pbi.objects[i]->type) {
      case OBJECT:
        DrawObject(pbi.objects[i]);
        DrawPermutation(pbi.objects[i]);
        DrawText(
          TextFormat("%d", pbi.objects[i]->id),
          pbi.objects[i]->position.x - 5, 
          pbi.objects[i]->position.y - 10, 10, 
          BLACK);
        break;
      case PERMUTANT:
        DrawPermutant(pbi.objects[i]);
        DrawPermutation(pbi.objects[i]);
        break;
      case QUERY:
        break;
    }
  }
  for (int i = 0; i < pbi.nPermutants; i++) {
    DrawText(TextFormat("%d", i), pbi.permutants[i]->position.x - 5, pbi.permutants[i]->position.y - 10, 10, BLACK);
  }

  if (pbi.queryAdded) {
    DrawQuery(pbi.query);
    DrawPermutation(pbi.query);
    if (state.queryType == RANGE)
      DrawCircleLinesV(pbi.query->position, state.queryRange, BLACK);
    else
      for (int i = 0; i < pbi.candidatesSize; i++){

      }

  }

}



int main(void) {
  Init();

  bool showMessageBox = false;

  Rectangle panelRec = {10, 300, 250, 400};
  Rectangle panelContentRec = {600, 200, 200, 400};
  Rectangle panelView = {0};
  Vector2 panelScroll = {99, -20};

  Rectangle panelRec2 = {10, 700, 250, 200};
  Rectangle panelContentRec2 = {600, 200, 200, 400};
  Rectangle panelView2 = {0};
  Vector2 panelScroll2 = {99, -20};

  while (!WindowShouldClose()) {

    state.mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), state.camera);


    InputHandler();
    BeginDrawing();
    BeginMode2D(state.camera);

    ClearBackground(RAYWHITE);

    DrawElement();

    updatePBI();

    if (pbi.queryAdded) {

      if (pbi.size > 0 && pbi.nPermutants > 0) {
        if (state.queryType == KNN) {
          KNNSearch(state.QueryK);
        } else {
          RangeSearch(state.queryRange);
        }

      }



        for (int i = 0; i < pbi.candidatesSize; i++) {
          if (pbi.candidates[i] != NULL) {
            Object *o = pbi.candidates[i];
            DrawRectangleLines(o->position.x - 10, o->position.y - 10, 20, 20, BLACK);
          } else {
            printf("Error: NULL candidates at index %d\n", i);
          }
        }


    }

    EndMode2D();

    DrawText("Press SPACE to change mode", 10, 10, 20, MAROON);
    DrawText(TextFormat("Mode: %s", mode[state.drawMode]), 10, 30, 20, MAROON); 
    DrawText(TextFormat("Objects: %d", pbi.size), 1700, 30, 20, BLACK);
    DrawText(TextFormat("Permutants: %d", pbi.nPermutants), 1700, 50, 20, BLACK);

    if (GuiButton(
      (Rectangle){80, 100, 120, 30}, 
      TextFormat((state.queryType == KNN) ? "KNN" : "RANGE"))) {
      state.queryType = (state.queryType == KNN) ? RANGE : KNN;
    }

    GuiSliderBar(
      (Rectangle){80, 140, 120, 30}, 
      "range", 
      TextFormat("%.2f ", state.queryRange), 
      &state.queryRange, 1, 500);

    GuiLabel(
      (Rectangle){80, 180, 90, 30}, 
      TextFormat("K = %d", state.QueryK));

    if(GuiButton((Rectangle){130, 180, 30, 30}, "+")) {
      state.QueryK++;
    }

    if(GuiButton((Rectangle){170, 180, 30, 30}, "-")) {
      if (state.QueryK > 1)
        state.QueryK--;
    }

    GuiSliderBar(
      (Rectangle){80, 220, 120, 30}, 
      NULL,
      TextFormat("%.2f%%", percentage), 
      &percentage, 0, 90);


    DrawText("(ELEMENT, SIMILARITY)", panelRec.x , panelRec.y - 20, 20, RED);

    GuiScrollPanel(panelRec, NULL, panelContentRec, &panelScroll, &panelView);

    BeginScissorMode(panelView.x, panelView.y, panelView.width, panelView.height);
    {

      // We have to expand the panel to fit the content
      panelContentRec.height = pbi.size * 25;

      for (int i = 0; i < pbi.size; i++) {

        const char *text = TextFormat(
          "%s%d", (pbi.objects[i]->type == OBJECT) ? "U" : "P", 
          (pbi.objects[i]->type == OBJECT) ? pbi.objects[i]->id : pbi.objects[i]->permutantIndex);

        DrawRectangleRec((Rectangle) {
          panelRec.x + 10 + panelScroll.x, 
          panelView.y + 10 + 20 * i + panelScroll.y, 
          300, 
          20},
                         Fade(BLACK, 0.1));

        int x = pbi.size * (percentage / 100);
        if (i < x) {
          DrawRectangleLines(
            panelRec.x + 10 + panelScroll.x, 
            panelView.y + 10 + 20 * i + panelScroll.y, 
            300, 
            20, 
            RED);


        } else {
          DrawRectangleLines(
            panelRec.x + 10 + panelScroll.x, 
            panelView.y + 10 + 20 * i + panelScroll.y, 
            300, 
            20, 
            BLACK);

        }
        GuiLabel(
          (Rectangle){
            panelRec.x + 20 + panelScroll.x, 
            panelView.y + 10 + 20 * i + panelScroll.y, 
            100, 
            20}, 
          text); 
        if (pbi.queryAdded){
          GuiLabel(
            (Rectangle){
              panelRec.x + 10 + panelScroll.x + 100, 
              panelView.y + 10 + 20 * i + panelScroll.y,
              100, 
              20}, 
            TextFormat("%d", pbi.objects[i]->spearmanRhoToQuery));

        }
      }
    }
    EndScissorMode();

    GuiScrollPanel(panelRec2, NULL, panelContentRec2, &panelScroll2, &panelView2);


    BeginScissorMode(panelView2.x, panelView2.y, panelView2.width, panelView2.height);
    {

      panelContentRec2.height = pbi.candidatesSize * 25;

      for (int i = 0; i < pbi.candidatesSize; i++) {

        const char *text = TextFormat(
          "%s%d", (pbi.candidates[i]->type == OBJECT) ? "U" : "P", 
          (pbi.candidates[i]->type == OBJECT) ? pbi.candidates[i]->id : pbi.candidates[i]->permutantIndex);


        GuiLabel(
          (Rectangle){
            panelRec2.x + 20 + panelScroll2.x, 
            panelView2.y + 10 + 20 * i + panelScroll2.y, 
            100, 
            20}, 
          text); 

      }
    }

    EndScissorMode();
    EndDrawing();
  }

  CloseWindow();
  FreePBI();
  return 0;

}
