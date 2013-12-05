/*
Lightweight Automated Planning Toolkit
Copyright (C) 2012
Miquel Ramirez <miquel.ramirez@rmit.edu.au>
Nir Lipovetzky <nirlipo@gmail.com>

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
*/

#ifndef __LANDMARK_GRAPH_MANAGER__
#define __LANDMARK_GRAPH_MANAGER__

#include <aptk/search_prob.hxx>
#include <aptk/heuristic.hxx>
#include <strips_state.hxx>
#include <strips_prob.hxx>
#include <landmark_graph.hxx>
#include <action.hxx>
#include <vector>
#include <deque>
#include <iosfwd>

namespace aptk {

namespace agnostic {



template <typename Search_Model >
class Landmarks_Graph_Manager  {
public:

	Landmarks_Graph_Manager( const Search_Model& prob, Landmarks_Graph* lg ) 
	:  m_strips_model( prob.task() )
	{
		m_graph = lg;
	}

	virtual ~Landmarks_Graph_Manager() {
	}

	
	void reset_graph(){
		m_graph->unconsume_all();
	}

	void update_graph( Bool_Vec_Ptr*& keep_consumed, Bool_Vec_Ptr*& keep_unconsumed ){
		if(keep_consumed)
			for( Bool_Vec_Ptr::iterator it = keep_consumed->begin(); it != keep_consumed->end(); it++)
				**it = true;
		if(keep_unconsumed)
			for( Bool_Vec_Ptr::iterator it = keep_unconsumed->begin(); it != keep_unconsumed->end(); it++)
				**it = false;
	}

	void undo_graph( Bool_Vec_Ptr*& undo_consumed, Bool_Vec_Ptr*& undo_unconsumed ){
		if(undo_consumed)
			for( Bool_Vec_Ptr::iterator it = undo_consumed->begin(); it != undo_consumed->end(); it++)
				**it = false;
		if(undo_unconsumed)
			for( Bool_Vec_Ptr::iterator it = undo_unconsumed->begin(); it != undo_unconsumed->end(); it++)
				**it = true;
	}

	void apply_action( Action_Idx a_idx, Bool_Vec_Ptr*& keep_consumed, Bool_Vec_Ptr*& keep_unconsumed ){

		const Fluent_Vec& add = m_strips_model.actions()[ a_idx ]->add_vec();
		const Fluent_Vec& del = m_strips_model.actions()[ a_idx ]->del_vec();		

		for(Fluent_Vec::const_iterator it_add = add.begin(); it_add != add.end(); it_add++){
			unsigned p = *it_add;
			
			if( m_graph->is_landmark(p) ){
				Landmarks_Graph::Node* n = m_graph->node(p);
				if( (! n->is_consumed()) && n->are_precedences_consumed() ){
					if( !keep_consumed ) keep_consumed = new Bool_Vec_Ptr;
					//					std::cout << "\t -- "<<p <<" - " << m_strips_model.fluents()[ p ]->signature() << std::endl;
					n->consume( );
					keep_consumed->push_back( n->is_consumed_ptr() );
				}
			}
		}

		for(Fluent_Vec::const_iterator it_del = del.begin(); it_del != del.end(); it_del++){
			unsigned p = *it_del;
			
			if( m_graph->is_landmark(p) ){
				Landmarks_Graph::Node* n = m_graph->node(p);
				if( n->is_consumed() && (! n->are_requirements_consumed() ) ){
					if( !keep_unconsumed ) keep_unconsumed = new Bool_Vec_Ptr;
					//					std::cout << "\t ++ "<<p <<" - " << m_strips_model.fluents()[ p ]->signature() << std::endl;
					n->unconsume( );
					keep_unconsumed->push_back( n->is_consumed_ptr() );
				}
			}
		}
	}

	void apply_state( const Fluent_Vec& fl, Bool_Vec_Ptr*& keep_consumed, Bool_Vec_Ptr*& keep_unconsumed ){

		for(Fluent_Vec::const_iterator it_fl = fl.begin(); it_fl != fl.end(); it_fl++){
			unsigned p = *it_fl;
			if( m_graph->is_landmark(p) ){
				
				Landmarks_Graph::Node* n = m_graph->node(p);
				if( (! n->is_consumed()) && n->are_precedences_consumed() ){
					if( !keep_consumed ) keep_consumed = new Bool_Vec_Ptr;
					n->consume( );
					keep_consumed->push_back( n->is_consumed_ptr() );
				}
			}
		}

	}
	
	
public:
	Landmarks_Graph*         graph(){ return m_graph; }
	const	STRIPS_Problem&	 problem() const			{ return m_strips_model; }

protected:

	const STRIPS_Problem&			m_strips_model;	
	Landmarks_Graph*                         m_graph;
};

}

}

#endif // landmark_graph_manager.hxx
