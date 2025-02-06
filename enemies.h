// enemies.h
#ifndef ENEMIES_H
#define ENEMIES_H




// Functions for enemy management
Enemy createRandomEnemy(Room *room, int roomWidth, int roomHeight);
int addEnemyToRoom(Room *room, Enemy enemy);
void populateEnemies(Room *rooms, int roomCount);
void moveEnemyTowardsPlayer(Enemy *enemy);
void handleEnemiesInRoom(Room *room);
void hitNearEnemies(Room *room, int x, int y);
void hitFarEnemies(Room *room, int x, int y);
#endif
