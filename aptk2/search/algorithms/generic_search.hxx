/*
Lightweight Automated Planning Toolkit (LAPKT)
Copyright (C) 2015

<contributors>
Miquel Ramirez <miquel.ramirez@gmail.com>
</contributors>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Additional note:

Concepts borrowed from Ethan Burn's heuristic search framework.
*/

#pragma once

#include <algorithm>
#include <memory>

#include <aptk2/search/interfaces/search_algorithm.hxx>
#include <aptk2/tools/logging.hxx>

namespace aptk {

	//! This class implements the classic blind search algorithm, GenericSearch
	template <typename NodeType,
	          typename OpenList,
	          typename ClosedList,
	          typename StateModel>
	class GenericSearch : public SearchAlgorithm< StateModel > {
	public:
		typedef SearchAlgorithm< StateModel >	BaseClass;
		typedef OpenList			OpenListType;
		typedef ClosedList			ClosedListType;
		typedef typename StateModel::StateType	State;

		typedef std::shared_ptr<NodeType>	NodePtrType;

		GenericSearch( const StateModel& _model ) :
			BaseClass( _model ) {
		}

		virtual ~GenericSearch() {}

		virtual bool search( const State& s, typename BaseClass::Plan& solution ) {
			NodePtrType n = std::make_shared<NodeType>( s );
			open.insert( n );
			BaseClass::generated++;

			while ( !open.is_empty() ) {
				NodePtrType current = open.get_next( );
				LPT_DEBUG("cout", *current);
				
				if ( BaseClass::model.goal( current->state ) ) { // Solution found, we're done
					LPT_INFO("cout", "Goal found");
					retrieve_solution( current, solution );
					return true;
				}

				// close the node before the actual expansion so that children which are identical
				// to 'current' get properly discarded
				closed.put( current );
				
				for ( const auto& a : BaseClass::model.applicable_actions( current->state ) ) {
					State s_a = BaseClass::model.next( current->state, a );
					NodePtrType succ = std::make_shared<NodeType>( std::move(s_a), a, current );
					if ( closed.check( *succ ) ) continue;
					open.insert( succ );
					BaseClass::generated++;
				}

				BaseClass::expanded++; // Count as an expansion
			}
			return false;
		}

		virtual void retrieve_solution( NodePtrType n, typename BaseClass::Plan& solution ) {
			NodePtrType tmp = n;
			while ( tmp->has_parent() ) {
				solution.push_back( tmp->action );
				tmp = tmp->parent;
			}
			std::reverse( solution.begin(), solution.end() );
		}


		OpenListType	open;
		ClosedListType	closed;
	};
}
