// File:        imglist.cpp
// Date:        2022-01-27 10:21
// Description: Contains partial implementation of ImgList class
//              for CPSC 221 2021W2 PA1
//              Function bodies to be completed by yourselves
//
// ADD YOUR FUNCTION IMPLEMENTATIONS IN THIS FILE
//

#include "imglist.h"

#include <iostream>

#include <math.h> // provides fmax, fmin, and fabs functions

/**************************
* MISCELLANEOUS FUNCTIONS *
**************************/

/*
* This function is NOT part of the ImgList class,
* but will be useful for one of the ImgList functions.
* Returns the "difference" between two hue values.
* PRE: hue1 is a double between [0,360).
* PRE: hue2 is a double between [0,360).
* 
* The hue difference is the absolute difference between two hues,
* but takes into account differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
double HueDiff(double hue1, double hue2) {
  return fmin(fabs(hue1 - hue2), fabs(360 + fmin(hue1, hue2) - fmax(hue1, hue2)));
}

/*********************
* CONSTRUCTORS, ETC. *
*********************/

/*
* Default constructor. Makes an empty list
*/
ImgList::ImgList() {
  // set appropriate values for all member attributes here
  northwest = NULL;
  southeast = NULL;
}

/*
* Creates a list from image data
* PRE: img has dimensions of at least 1x1
*/
ImgList::ImgList(PNG& img) {
  // build the linked node structure and set the member attributes appropriately
  // case 1: if img is 1x1

  northwest = new ImgNode();  // pointer to the absolute top left corner of the ImgList 

  ImgNode* topleft = northwest;  // pointer to the ImgNode above the first ImgNode of each row
  ImgNode* topright = new ImgNode();  // pointer to the ImgNode above the last ImgNode of each row

  for (unsigned int y = 0; y < img.height(); y++) {
    // special case: construction of first row is slightly different
    if (y == 0) {  
      ImgNode* left = topleft;
      ImgNode* right = topright;
      left->colour = *img.getPixel(0, y);
      right->colour = *img.getPixel(img.width()-1, y);

      for (unsigned int x = 1; x < img.width() - 1; x++) {
        HSLAPixel* pixel = img.getPixel(x, y);
        insertTop(left, right, pixel);
        left = left->east;
      }

    }

    // construction of rows below the top row
    else {
      ImgNode* left = new ImgNode();
      ImgNode* right = new ImgNode();
      ImgNode* top = topleft->east;
      left->colour = *img.getPixel(0, y);
      right->colour = *img.getPixel(img.width()-1, y);
      left->north = topleft;
      right->north = topright;
      topleft->south = left;
      topright->south = right;
      
      for (unsigned int x = 1; x < img.width() - 1; x++) {
        HSLAPixel* pixel = img.getPixel(x, y);
        insert(left, right, top, pixel);
        left = left->east;
        top = top->east;
      }
      //update the pointer to the top of the row 
      topleft = topleft->south; 
      topright = topright->south;

    }
  }
  // construct southeast to be the last node in the ImgList
  southeast = topright; 
  
}


/*
* Insert a node between 2 nodes that are next to each other in a row
* PRE: 
*
*/
void ImgList::insert(ImgNode* left, ImgNode* right, ImgNode* top, HSLAPixel* pixel) {

  ImgNode* node = new ImgNode();

  // Link node to the middle of left and right node
  node->west = left;
  node->east = right;

  left->east= node;
  right->west = node;
  
  // Link node to the node above it
  node->north = top;
  top->south = node;

  // colour the node
  node->colour = *pixel;

}

/*
* Insert a node between 2 nodes that are next to each other in the top row of an ImgList
*/
void ImgList::insertTop(ImgNode* left, ImgNode* right, HSLAPixel* pixel) {

  ImgNode* node = new ImgNode();

  // Link node to the middle of left and right node
  node->west = left;
  node->east = right;

  left->east= node;
  right->west = node;

  // colour the node
  node->colour = *pixel;
}

/*
* Copy constructor.
* Creates this this to become a separate copy of the data in otherlist
*/
ImgList::ImgList(const ImgList& otherlist) {
  // build the linked node structure using otherlist as a template
  Copy(otherlist);
}

/*
* Assignment operator. Enables statements such as list1 = list2;
*   where list1 and list2 are both variables of ImgList type.
* POST: the contents of this list will be a physically separate copy of rhs
*/
ImgList& ImgList::operator=(const ImgList& rhs) {
  // Re-build any existing structure using rhs as a template
  
  if (this != &rhs) { // if this list and rhs are different lists in memory
    // release all existing heap memory of this list
    Clear();    
    
    // and then rebuild this list using rhs as a template
    Copy(rhs);
  }
  
  return *this;
}

/*
* Destructor.
* Releases any heap memory associated with this list.
*/
ImgList::~ImgList() {
  // Ensure that any existing heap memory is deallocated
  Clear();
}

/************
* ACCESSORS *
************/

/*
* Returns the horizontal dimension of this list (counted in nodes)
* Note that every row will contain the same number of nodes, whether or not
*   the list has been carved.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionX() const {
  // replace the following line with your implementation
  int x = 1;
  ImgNode* loopX = northwest;
  while (loopX->east) {
    loopX = loopX->east;
    x++;
  }

  return x;
}

/*
* Returns the vertical dimension of the list (counted in nodes)
* It is useful to know/assume that the grid will never have nodes removed
*   from the first or last columns. The returned value will thus correspond
*   to the height of the PNG image from which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   y dimension.
*/
unsigned int ImgList::GetDimensionY() const {
  // replace the following line with your implementation
  int y = 1;
  ImgNode* loopY = northwest;
  while (loopY->south) {
    loopY = loopY->south;
    y++;
  }

  return y;
}

/*
* Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
* The returned value will thus correspond to the width of the PNG image from
*   which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionFullX() const {
  ImgNode* currNode = northwest;
  int width = 1;

  while (currNode->east) {
    width += currNode->skipright;
    width += 1;
    currNode = currNode->east;
  }

  return width;
}


/*
* Returns a pointer to the node which best satisfies the specified selection criteria.
* The first and last nodes in the row cannot be returned.
* PRE: rowstart points to a row with at least 3 physical nodes
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rowstart - pointer to the first node in a row
* PARAM: selectionmode - criterion used for choosing the node to return
*          0: minimum luminance across row, not including extreme left or right nodes
*          1: node with minimum total of "hue difference" with its left neighbour and with its right neighbour.
*        In the (likely) case of multiple candidates that best match the criterion,
*        the left-most node satisfying the criterion (excluding the row's starting node)
*        will be returned.
* A note about "hue difference": For PA1, consider the hue value to be a double in the range [0, 360).
* That is, a hue value of exactly 360 should be converted to 0.
* The hue difference is the absolute difference between two hues,
* but be careful about differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
ImgNode* ImgList::SelectNode(ImgNode* rowstart, int selectionmode) {

  ImgNode* currNode = rowstart->east;
  ImgNode* selectedNode = currNode;

  // selectionmode 0
  if (selectionmode == 0) {
    double lMin = currNode->colour.l;

    while (currNode->east->east) {
      if (currNode->east->colour.l < lMin) {
        lMin = currNode->east->colour.l;
        selectedNode = currNode->east;
      }
      currNode = currNode->east;
    }
    
  
  // selectionmode 1
  } else {
    double hMin = HueDiff(currNode->colour.h, currNode->west->colour.h) + HueDiff(currNode->colour.h, currNode->east->colour.h); 

    while (currNode->east->east) {
      currNode = currNode->east;
      double hueDiff = HueDiff(currNode->colour.h, currNode->west->colour.h) + HueDiff(currNode->colour.h, currNode->east->colour.h);
      
      if (hueDiff < hMin - 0.10) { // 0.10 makes sure that arithmetric floating point inaccuracies are taken care of 
        hMin = hueDiff;
        selectedNode = currNode;
      }
    }
  }
  
  return selectedNode;
}


/*
* Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
* PRE: fillmode is an integer in the range of [0,2]
* PARAM: fillgaps - whether or not to fill gaps caused by carving
*          false: render one pixel per node, ignores fillmode
*          true: render the full width of the original image,
*                filling in missing nodes using fillmode
* PARAM: fillmode - specifies how to fill gaps
*          0: solid, uses the same colour as the node at the left of the gap
*          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
*             Note that "average" for hue will use the closer of the angular distances,
*             e.g. average of 10 and 350 will be 0, instead of 180.
*             Average of diametric hue values will use the smaller of the two averages
*             e.g. average of 30 and 210 will be 120, and not 300
*                  average of 170 and 350 will be 80, and not 260
*          2: *** OPTIONAL - FOR BONUS ***
*             linear gradient between the colour (all channels) of the nodes at the left and right of the gap
*             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
*             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
*             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
*             Like fillmode 1, use the smaller difference interval for hue,
*             and the smaller-valued average for diametric hues
*/
PNG ImgList::Render(bool fillgaps, int fillmode) const {
  unsigned int width1 = GetDimensionX();
  unsigned int width2 = GetDimensionFullX();
  unsigned int height = GetDimensionY();

  PNG outpng;
  if (fillgaps) {
    outpng.resize(width2, height);
  } else {
    outpng.resize(width1, height);
  }
  
  ImgNode* currRow = northwest;
  ImgNode* currNode = northwest;

  // fillgaps is 0
  if (!fillgaps) {

    unsigned int y = 0;
    while (y < height) {
      unsigned int x = 0;
      while (x < width1) {
        HSLAPixel* pixel = outpng.getPixel(x, y);
        *pixel = currNode->colour;
        x++;
        // break early to prevent currNode to point to NULL
        if (x == width1) {
          break;
        }
        // update to next node in the row
        currNode = currNode->east;
      }

      y++;
      // break early to prevent currRow to point to NULL
      if (y == height) {
        break;
      }

      // update node for the next row
      currRow = currRow->south;
      currNode = currRow;
    }
  
  // fillgaps is 1 and fillmode is 0
  } else {
    if (fillmode == 0) {

      unsigned int y = 0;
      while (y < height) {
        unsigned int x = 0;
        while(x < width2) {
          HSLAPixel* pixel = outpng.getPixel(x, y);
          *pixel = currNode->colour;
          x++;
          // keep adding pixels based on the skipvalues
          for (unsigned int i = 0; i < currNode->skipright; i++) {
            HSLAPixel* pixel = outpng.getPixel(x, y);
            *pixel = currNode->colour;
            x++;
          }
          // break early to prevent currNode to update to NULL
          if (x == width2) {
            break;
          }
          currNode = currNode->east;
        }
        // update to next row and break early to prevent currNode to update to NULL
        y++;
        if (y == height) {
          break;
        }
        currRow = currRow->south;
        currNode = currRow;
      }
    // fillmode is 1
    } else if (fillmode == 1) {

      unsigned int y = 0;
      while (y < height) {
        unsigned int x = 0;
        while(x < width2) {
          HSLAPixel* pixel = outpng.getPixel(x, y);
          *pixel = currNode->colour;
          x++;
          // keep adding pixels based on the skipvalues
          for (unsigned int i = 0; i < currNode->skipright; i++) {
            HSLAPixel* pixel = outpng.getPixel(x, y);
            pixel->h = HueAverage(currNode->colour.h, currNode->east->colour.h);
            pixel->s = (currNode->colour.s + currNode->east->colour.s) / 2.0;
            pixel->l = (currNode->colour.l + currNode->east->colour.l) / 2.0;
            pixel->a = (currNode->colour.a + currNode->east->colour.a) / 2.0;
            x++;
          }
          // break early to prevent currNode to update to NULL
          if (x == width2) {
            break;
          }
          currNode = currNode->east;
        }
        // update to next row and break early to prevent currNode to update to NULL
        y++;
        if (y == height) {
          break;
        }
        currRow = currRow->south;
        currNode = currRow;
      }

    } 
  }
  
  return outpng;
}

/************
* MODIFIERS *
************/

/*
* Removes exactly one node from each row in this list, according to specified criteria.
* The first and last nodes in any row cannot be carved.
* PRE: this list has at least 3 nodes in each row
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: selectionmode - see the documentation for the SelectNode function.
* POST: this list has had one node removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(int selectionmode) {
  ImgNode* currRow = northwest;

  while (currRow) {
    ImgNode* removeNode = SelectNode(currRow, selectionmode);
    
    // relink left and right ImgNodes and update skip value by 1
    removeNode->west->east = removeNode->east;
    removeNode->east->west = removeNode->west;
    removeNode->west->skipright += 1;
    removeNode->east->skipleft += 1;

    // take the removed node's skip values into account 
    removeNode->west->skipright += removeNode->skipright;
    removeNode->east->skipleft += removeNode->skipleft;
 
    // check if the ImgNode is in the first or last row (north or south points to NULL)
    if (removeNode->north) {
      removeNode->north->south = removeNode->south;
      removeNode->north->skipdown += 1;
      removeNode->north->skipdown += removeNode->skipdown;
    }
    if (removeNode->south) {
      removeNode->south->north = removeNode->north;
      removeNode->south->skipup += 1;
      removeNode->south->skipup += removeNode->skipup;
    }

    delete removeNode;
    removeNode = NULL;

    if (!currRow->south) {
      break;
    }
    currRow = currRow->south;
  }

}

// note that a node on the boundary will never be selected for removal
/*
* Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
* based on specific selection criteria.
* Note that this should remove one node from every row, repeated "rounds" times,
* and NOT remove "rounds" nodes from one row before processing the next row.
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rounds - number of nodes to remove from each row
*        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
*        i.e. Ensure that the final list has at least two nodes in each row.
* POST: this list has had "rounds" nodes removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(unsigned int rounds, int selectionmode) {
  for (unsigned int i = 0; i < rounds; i++)  {
    // check if 'rounds' exceeds width -2
    if (northwest->east->east) {
      Carve(selectionmode);
    } else {
      return;
    }
  }

}


/*
* Helper function deallocates all heap memory associated with this list,
* puts this list into an "empty" state. Don't forget to set your member attributes!
* POST: this list has no currently allocated nor leaking heap memory,
*       member attributes have values consistent with an empty list.
*/
void ImgList::Clear() {
  /*
  if (!northwest) {
    return;
  }

  ImgNode* currNode = northwest;
  ImgNode* nextRow = northwest;

  do {

    // special case: if there is only one node for every row
    if (!currNode->east) {
      ImgNode* temp = currNode;
      currNode = nextRow;
      delete temp;
      temp = NULL;
    
    } else {
      do {
        ImgNode* temp = currNode;
        currNode = currNode->east;
        delete temp;
        temp = NULL;
      } while (currNode->east); 
      
      currNode = nextRow;
    }

    if (!nextRow->south) {
      return;
    } else {
      nextRow = nextRow->south;
    }

  } while (nextRow->south);
  */
}

/* ************************
*  * OPTIONAL - FOR BONUS *
** ************************
* Helper function copies the contents of otherlist and sets this list's attributes appropriately
* PRE: this list is empty
* PARAM: otherlist - list whose contents will be copied
* POST: this list has contents copied from by physically separate from otherlist
*/
void ImgList::Copy(const ImgList& otherlist) {
  // add your implementation here
}

/*************************************************************************************************
* IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
*************************************************************************************************/

/* Note that "average" for hue will use the closer of the angular distances,
*  e.g. average of 10 and 350 will be 0, instead of 180.
*  Average of diametric hue values will use the smaller of the two averages
*  e.g. average of 30 and 210 will be 120, and not 300
*  average of 170 and 350 will be 80, and not 260
*/                
double ImgList::HueAverage(double hue1, double hue2) const{
  double hueleft = hue1;
  double hueright = hue2;
  double hueaverage = 0.0;

  // edge cases for calculating average hue 
  if (hueleft == 360) {
    hueleft = 0.0;
  }
  if (hueright == 360) {
    hueright = 0.0;
  }
  
  double huediff = HueDiff(hueleft, hueright);
  if (fmin(hue1, hue2) + huediff == fmax(hue1, hue2)) {
    hueaverage = huediff / 2.0 + fmin(hueleft, hueright);
  } else {
    hueaverage = huediff / 2.0 + fmax(hueleft, hueright);
    if (hueaverage >= 360) {
      hueaverage -= 360;
    }
  }

  return hueaverage;
}
