// Daren Palmer 2023 - This file contains the main functions for the robot's movement, status, and also renders an ascii grid of the robot's current position.
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "graphics.h"
#include <string.h>

struct marker
{
  int x;
  int y;
};

enum direction
{
  Top,
  Bottom,
  Right,
  Left
};

struct Game
{
  int nMarkers;
  struct marker *markers;
  char grid[10][10];
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
  void (*right)(struct robot *robot);
};

// the forward function describes the robot's movement in the direction it is facing
// it also updates the robot's position on the grid and renders it in a colour denoting its state, returns a [x, y] array

void forward(struct robot *robot, char grid[10][10])
{
  int x = robot->x;
  int y = robot->y;
  char prev = grid[x][y];

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

  // update grid, if there was a home here before, replace it
  if (robot->homeX == x && robot->homeY == y)
  {
    strcpy(&grid[x][y], "H");
  }
  else if (strcmp(&prev, "M") == 0)
  {
    strcpy(&grid[x][y], "M");
  }
  else
  {
    strcpy(&grid[x][y], "N");
  }

  strcpy(&grid[robot->x][robot->y], "R");

  sleep(200);
}

void right(struct robot *robot)
{
  // turn right
  switch (robot->facing)
  {
  case Top:
    robot->facing = Right;
    break;
  case Bottom:
    robot->facing = Left;
    break;
  case Right:
    robot->facing = Bottom;
    break;
  case Left:
    robot->facing = Top;
    break;
  default:
    printf("Error in right function");
    break;
  }
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

void printGrid(char grid[10][10], struct robot robot)
{
  for (int i = 0; i < 10; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      {
        // print the row

        if (grid[i][j] == 'N') // if the grid is empty
        {
          setColour(white);
        }

        else if (grid[i][j] == 'R') // if the grid contains the robot
        {
          if (robot.carryingMarker)
          {
            setColour(green);
          }

          else
          {
            setColour(yellow);
          }
        }
        else if (grid[i][j] == 'H') // if the grid contains the home
        {
          setColour(red);
        }
        else if (grid[i][j] == 'M') // if the grid contains a marker
        {
          setColour(blue);
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

int main()
{
  // use this to make a virtual grid to keep account of the colours to use in the forward function
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
      .facing = Right,
      .forward = forward,
      .right = right,
  };

  // initialize the marker

  struct marker marker1 = {
      .x = 5,
      .y = 5,
  };

  struct marker marker2 = {
      .x = 5,
      .y = 0,
  };

  struct marker markers[] = {marker1, marker2};

  int nMarkers = sizeof(markers) / sizeof(markers[0]);

  for (int i = 0; i < nMarkers; i++)
  {
    strcpy(&grid[markers[i].x][markers[i].y], "M");
  }

  strcpy(&grid[bot.x][bot.y], "R");
  strcpy(&grid[bot.homeX][bot.homeY], "H");

  struct Game game = {
      .nMarkers = nMarkers,
      .markers = markers,
      .grid = grid,
  };

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

  while (game.nMarkers > 0)
  {
    // if the robot is at home and not carrying a marker, go pick one up
    int hasMarker = bot.carryingMarker;
    if (hasMarker)
    {
      // go home using homeX and homeY and robot x, y
      if (atHome(&bot))
      {
        bot.carryingMarker = 0;
        game.nMarkers--;
      }
      else
      {
        // use the history to go home
        for (int i = 0; i < MAX_HISTORY_SIZE && history[i] != -1; i++)
        {
          int dir = directionToInt(bot.facing);
          switch (history[i])
          // here we invert the direction the robot is facing to go home
          {
          case 0:
            bot.facing = Bottom;
            break;
          case 1:
            bot.facing = Top;
            break;
          case 2:
            bot.facing = Left;
            break;
          case 3:
            bot.facing = Right;
            break;
          default:
            break;
          }
          bot.forward(&bot, grid);

          // clear the history entry
          history[i] = -1;
          printGrid(grid, bot);
        }
      }
    }

    else
    {
      // check if there is a marker in the current position
      int markerHere = 0;

      for (int i = 0; i < nMarkers; i++)
      {
        if (bot.x == markers[i].x && bot.y == markers[i].y)
        {
          markerHere = 1;
          bot.carryingMarker = 1;
          grid[markers[i].x][markers[i].y] = 'N';
          markers[i].x = -1;
          break;
        }
      }

      // if there isn't we keep traversing the grid

      if (markerHere == 0)
      {
        // check if the robot is facing a wall
        int canMoveForward =
            (bot.facing == Top && bot.y > 0) ||
                    (bot.facing == Bottom && bot.y < 9) ||
                    (bot.facing == Right && bot.x < 9) ||
                    (bot.facing == Left && bot.x > 0)
                ? 1
                : 0;

        if (canMoveForward)
        {
          bot.forward(&bot, grid);

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
          printGrid(grid, bot);
        }
        else
        {
          bot.right(&bot);
        }
      }
    }
  }
  return 0;
}