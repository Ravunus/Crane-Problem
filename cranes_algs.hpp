///////////////////////////////////////////////////////////////////////////////
// cranes_algs.hpp
//
// Algorithms that solve the crane unloading problem.
//
// All of the TODO sections for this project reside in this file.
//
// This file builds on crane_types.hpp, so you should familiarize yourself
// with that file before working on this file.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cassert>
#include <optional>
#include <vector>

#include "cranes_types.hpp"

namespace cranes {

// Solve the crane unloading problem for the given grid, using an exhaustive
// optimization algorithm.
//
// This algorithm is expected to run in exponential time, so the grid's
// width+height must be small enough to fit in a 64-bit int; this is enforced
// with an assertion.
//
// The grid must be non-empty.
path crane_unloading_exhaustive(const grid& setting) {
  coordinate rows = setting.rows();
  coordinate columns = setting.columns();
  coordinate total_moves = rows + columns - 2;

  // Initialize the best path found so far.
  path best_path(setting);

  // Loop through all possible move sequences.
  for (coordinate i = 0; i < (1 << total_moves); ++i) {
    path current_path(setting);

    // Generate the current move sequence from the binary representation.
    for (coordinate move = 0; move < total_moves; ++move) {
      step_direction direction;
      if (i & (1 << move)) {
        direction = STEP_DIRECTION_SOUTH;
      } else {
        direction = STEP_DIRECTION_EAST;
      }

      // Check if the current step is valid.
      if (current_path.is_step_valid(direction)) {
        current_path.add_step(direction);
      } else {
        // Skip the invalid path.
        break;
      }
    }

    // Update the best path if the current path has more cranes reached.
    if (current_path.total_cranes() > best_path.total_cranes()) {
      best_path = current_path;
    }
  }

  return best_path;
}


// Solve the crane unloading problem for the given grid, using a dynamic
// programming algorithm.
//
// The grid must be non-empty.
path crane_unloading_dyn_prog(const grid& setting) {

  // grid must be non-empty.
  assert(setting.rows() > 0);
  assert(setting.columns() > 0);


  using cell_type = std::optional<path>;

  std::vector<std::vector<cell_type> > A(setting.rows(),
                                        std::vector<cell_type>(setting.columns()));

  A[0][0] = path(setting);
  assert(A[0][0].has_value());

  for (coordinate r = 0; r < setting.rows(); ++r) {
    for (coordinate c = 0; c < setting.columns(); ++c) {

      if (setting.get(r, c) == CELL_BUILDING){
        A[r][c].reset();
        continue;
        }

    cell_type from_above = std::nullopt;
    cell_type from_left = std::nullopt;

	    // TODO: implement the dynamic programming algorithm, then delete this
      // comment.
      // Compute from_above if we are not on the top edge.
      if (r > 0) {
        from_above = A[r - 1][c];
      }
      if (c > 0) {
        from_left = A[r][c - 1];
      }

      if (from_above && from_left) {
        if ((*from_above).total_cranes() < (*from_left).total_cranes()) {
          A[r][c] = *from_above;
          A[r][c]->add_step(STEP_DIRECTION_SOUTH);
        } else {
          A[r][c] = *from_left;
          A[r][c]->add_step(STEP_DIRECTION_EAST);
        }
      } else if (from_above) {
        A[r][c] = *from_above;
        A[r][c]->add_step(STEP_DIRECTION_SOUTH);
      } else if (from_left) {
        A[r][c] = *from_left;
        A[r][c]->add_step(STEP_DIRECTION_EAST);
      }
    }
  }

  assert(A[setting.rows() - 1][setting.columns() - 1].has_value());
  return *A[setting.rows() - 1][setting.columns() - 1];
  }

}
