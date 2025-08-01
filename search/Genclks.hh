// OpenSTA, Static Timing Analyzer
// Copyright (c) 2025, Parallax Software, Inc.
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
// 
// The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software.
// 
// Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 
// This notice may not be removed or altered from any source distribution.

#pragma once

#include "Map.hh"
#include "Transition.hh"
#include "NetworkClass.hh"
#include "Graph.hh"
#include "SdcClass.hh"
#include "SearchClass.hh"
#include "StaState.hh"

namespace sta {

class GenclkInfo;
class BfsFwdIterator;
class BfsBkwdIterator;
class SearchPred;
class TagGroupBldr;

typedef std::pair<const Clock*,const Pin *> ClockPinPair;

class ClockPinPairLess
{
public:
  bool operator()(const ClockPinPair &pair1,
		  const ClockPinPair &pair2) const;
};

typedef Map<Clock*, GenclkInfo*> GenclkInfoMap;
typedef Map<ClockPinPair, std::vector<Path>, ClockPinPairLess> GenclkSrcPathMap;
typedef std::map<Vertex*, std::vector<const Path*>, VertexIdLess> VertexGenclkSrcPathsMap;

class Genclks : public StaState
{
public:
  Genclks(StaState *sta);
  ~Genclks();
  void clear();
  void ensureInsertionDelays();
  VertexSet *fanins(const Clock *clk);
  void findLatchFdbkEdges(const Clock *clk);
  EdgeSet *latchFdbkEdges(const Clock *clk);
  void checkMaster(Clock *gclk);
  void ensureMaster(Clock *gclk);
  // Generated clock insertion delay.
  Arrival insertionDelay(const Clock *clk,
			 const Pin *pin,
			 const RiseFall *rf,
			 const EarlyLate *early_late,
			 const PathAnalysisPt *path_ap) const;
  // Generated clock source path for a clock path root.
  const Path *srcPath(const Path *clk_path) const;
  // Generated clock source path.
  const Path *srcPath(const ClockEdge *clk_edge,
                      const Pin *src_pin,
                      const PathAnalysisPt *path_ap) const;
  const Path *srcPath(const Clock *clk,
                      const Pin *src_pin,
                      const RiseFall *rf,
                      const PathAnalysisPt *path_ap) const;
  Vertex *srcPath(const Pin *pin) const;
  Level clkPinMaxLevel(const Clock *clk) const;
  void copyGenClkSrcPaths(Vertex *vertex,
			  TagGroupBldr *tag_bldr);

private:
  void findInsertionDelays();
  GenclkInfo *genclkInfo(const Clock *gclk) const;
  void clearSrcPaths();
  void recordSrcPaths(Clock *gclk);
  void findInsertionDelays(Clock *gclk);
  void seedClkVertices(Clock *clk,
		       BfsBkwdIterator &iter,
		       VertexSet *fanins);
  size_t srcPathIndex(const RiseFall *clk_rf,
                      const PathAnalysisPt *path_ap) const;
  bool matchesSrcFilter(Path *path,
			const Clock *gclk) const;
  void seedSrcPins(Clock *gclk,
		   FilterPath *src_filter,
		   BfsFwdIterator &insert_iter);
  void findSrcArrivals(Clock *gclk,
		       BfsFwdIterator &insert_iter,
		       GenclkInfo *genclk_info);
  virtual FilterPath *makeSrcFilter(Clock *gclk);
  void deleteGenClkInfo();
  virtual Tag *makeTag(const Clock *gclk,
		       const Clock *master_clk,
		       const Pin *master_pin,
		       const RiseFall *rf,
		       FilterPath *src_filter,
                       Arrival insert,
		       const PathAnalysisPt *path_ap);
  void seedSrcPins(Clock *clk,
		   BfsBkwdIterator &iter);
  void findInsertionDelay(Clock *gclk);
  GenclkInfo *makeGenclkInfo(Clock *gclk);
  FilterPath *srcFilter(Clock *gclk);
  void findFanin(Clock *gclk,
		 // Return value.
		 VertexSet *fanins);
  void findLatchFdbkEdges(const Clock *clk,
			  GenclkInfo *genclk_info);
  void findLatchFdbkEdges(Vertex *vertex,
			  Level gclk_level,
			  SearchPred &srch_pred,
			  VertexSet &path_vertices,
			  VertexSet &visited_vertices,
			  EdgeSet *&fdbk_edges);

  bool found_insertion_delays_;
  GenclkSrcPathMap genclk_src_paths_;
  GenclkInfoMap genclk_info_map_;
  VertexGenclkSrcPathsMap vertex_src_paths_map_;
};

} // namespace
