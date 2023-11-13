// Daren Palmer 2023 - This file contains the main functions for the robot's movement, status, and also renders an ascii grid of the robot's current position.
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "graphics.h"
#include <string.h>

// on windows, use the following line
// #include <Windows.h>

struct marker
{
  int x;
  int y;
  int pickedUp;
};

enum direction
{
  Top,
  Bottom,
  Right,
  Left
};

struct robot
{
  int x;
  int y;
  int homeX;
  int homeY;
  int carryingMarker;
  enum direction facing;
  void (*traversalFunction)(struct robot *robot);
  void (*forward)(struct robot *robot, char grid[10][10]);
};

// the forward function describes the robot's movement in the direction it is facing
// it also updates the robot's position on the grid and renders it in a colour denoting its state, returns a [x, y] array

void forward(struct robot *robot, char grid[10][10])
{
  int x = robot->x;
  int y = robot->y;

  // advance in the direction the robot is facing
  switch (robot->facing)
  {
  case Top:
    robot->y--;
    break;
  case Bottom:
    robot->y++;
    break;
  case Right:
    robot->x++;
    break;
  case Left:
    robot->x--;
    break;
  default:
    printf("Error in forward function");
    break;
  }

  strcpy(&grid[robot->homeX][robot->homeY], "H");
  strcpy(&grid[robot->x][robot->y], "R");
  strcpy(&grid[x][y], "N");
}

int atHome(struct robot *robot)
{
  if (robot->x == robot->homeX && robot->y == robot->homeY)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void printGrid(char grid[10][10], struct robot *robot)
{
  for (int i = 0; i < 10; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      {
        if (i == robot->x && j == robot->y) // if the cell contains the robot

        {
          if (robot->carryingMarker)
          {
            setColour(green);
          }
          else
          {
            setColour(yellow);
          }
        }

        else if (i == robot->homeY && j == robot->homeX) // if the cell contains the robot's home

        {
          setColour(red);
        }

        else if (grid[i][j] == 'M') // if the cell contains a marker
        {
          setColour(blue);
        }

        else
        {
          setColour(white);
        }

        fillRect(i * 20, j * 20, 20, 20);
        // draw lines
        setColour(black);
        drawLine(i * 20, j * 20, i * 20, j * 20 + 20);
        drawLine(i * 20, j * 20, i * 20 + 20, j * 20);
        setColour(white);
      }
    }
  }
}

int directionToInt(enum direction dir)
{
  switch (dir)
  {
  case Top:
    return 0;
    break;
  case Bottom:
    return 1;
    break;
  case Right:
    return 2;
    break;
  case Left:
    return 3;
    break;
  default:
    return -1;
    break;
  }
}

void traverse(struct robot *r, char grid[10][10])
{
  enum direction dir = r->x % 2 == 0 ? Bottom : Top;
  int canMove = dir == Top ? r->y > 0 : r->y < 9;
  if (canMove)
  {
    r->facing = dir;
    r->forward(r, grid);
  }
  else
  {
    // we turn right to the next column
    r->facing = Right;
    r->forward(r, grid);
  }
}
int main()
{
  // use this to make a virtual grid to keep account of the colours to use in the printGrid function
  char grid[10][10] = {
      {'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'},
      {'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'},
      {'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'},
      {'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'},
      {'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'},
      {'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'},
      {'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'},
      {'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'},
      {'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'},
      {'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'},
  };

  // initialize the robot

  struct robot bot = {
      .x = 0,
      .y = 0,
      .homeX = 0,
      .homeY = 0,
      .carryingMarker = 0,
      .facing = Bottom,
      .forward = forward,

  };

  // initialize the marker

  struct marker marker1 = {
      .x = 0,
      .y = 3,
      .pickedUp = 0,
  };

  struct marker marker2 = {
      .x = 4,
      .y = 4,
      .pickedUp = 0,
  };

  struct marker markers[] = {marker1, marker2};

  int nMarkers = sizeof(markers) / sizeof(markers[0]);

  for (int i = 0; i < nMarkers; i++)
  {
    strcpy(&grid[markers[i].x][markers[i].y], "M");
  }

  strcpy(&grid[bot.x][bot.y], "R");
  strcpy(&grid[bot.homeX][bot.homeY], "H");

  // we can't trivialy store enum values in an array
  // -1 - empty
  // 0 - Top
  // 1 - Bottom
  // 2 - Right
  // 3 - Left

  int MAX_HISTORY_SIZE = 100;

  int history[MAX_HISTORY_SIZE];

  // Initialize the history array
  for (int i = 0; i < MAX_HISTORY_SIZE; i++)
  {
    history[i] = -1;
  }

  while (nMarkers > 0)
  {
    printGrid(grid, &bot);

    // if the robot is at home and not carrying a marker, go pick one up
    if (bot.carryingMarker)
    {
      if (atHome(&bot))
      {
        bot.carryingMarker = 0;
        nMarkers--;
      }
      else
      {
        // use the history to go home
        // get last entry in history that is not -1
        int i;
        enum direction dir;
        for (i = MAX_HISTORY_SIZE - 1; i >= 0; i--)
        {
          if (history[i] != -1)
          {
            switch (history[i])
            {
            case 0:
              dir = Bottom;
              break;
            case 1:
              dir = Top;
              break;
            case 2:
              dir = Left;
              break;
            case 3:
              dir = Right;
              break;
            default:
              break;
            }

            bot.facing = dir;
            bot.forward(&bot, grid);
            history[i] = -1;
            break;
          }
        }
      }
    }

    else
    {
      traverse(&bot, grid);
      printGrid(grid, &bot);

      // Add the direction the robot is facing to the history
      int i;
      for (i = 0; i < MAX_HISTORY_SIZE; i++)
      {
        if (history[i] == -1)
        {
          history[i] = directionToInt(bot.facing);
          break;
        }
      }
      // Check if the history array is full
      if (i == MAX_HISTORY_SIZE)
      {
        // Shift the history array to remove the oldest entry
        memmove(&history[0], &history[1], (MAX_HISTORY_SIZE - 1) * sizeof(int));
        // Add the new entry at the end
        history[MAX_HISTORY_SIZE - 1] = directionToInt(bot.facing);
      }

      // Check if the robot is on a marker

      for (int i = 0; i <= nMarkers; i++)
      {
        if (bot.x == markers[i].x && bot.y == markers[i].y && markers[i].pickedUp == 0)
        {
          bot.carryingMarker = 1;
          markers[i].pickedUp = 1;
        }
      }
    }
    sleep(100);
  }
  return 0;
}