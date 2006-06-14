// Copyright (c) 1997  Tel-Aviv University (Israel).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Baruch Zukerman <baruchzu@post.tau.ac.il>

/*! \file
 * Definition of the Arr_construction<Arrangement> class.
 */

#ifndef CGAL_ARR_CONSTRUCTION_H
#define CGAL_ARR_CONSTRUCTION_H

#include <CGAL/Sweep_line_2.h>
#include <CGAL/Sweep_line_2/Arr_construction_event.h>
#include <CGAL/Sweep_line_2/Arr_construction_curve.h>
#include <CGAL/Sweep_line_2/Arr_construction_visitor.h>
#include <CGAL/Sweep_line_2/Sweep_line_2_utils.h>
#include <CGAL/assertions.h>
#include <list>
#include <algorithm>

CGAL_BEGIN_NAMESPACE

/*! \class
 * An auxiliray class for performing aggragated insertion of a range of curves
 * into an arrangement using the sweep-line algorithm.
 */
template <class Arrangement_>
class Arr_construction 
{
  typedef Arrangement_                              Arrangement_2;
  typedef typename Arrangement_2::Halfedge_handle   Halfedge_handle;
  typedef typename Arrangement_2::Edge_iterator     Edge_iterator;
  typedef typename Arrangement_2::Vertex_handle     Vertex_handle;
  typedef typename Arrangement_2::Vertex_iterator   Vertex_iterator;
  typedef typename Arrangement_2::Traits_2          Traits_2;
  typedef Arr_construction_curve<Traits_2>          Subcurve; 
  typedef Arr_construction_event<Traits_2,
                               Subcurve,
                               Halfedge_handle>     Event;
  typedef typename Traits_2::X_monotone_curve_2     X_monotone_curve_2;
  typedef typename Traits_2::Point_2                Point_2;

  typedef Arr_construction_visitor<Traits_2,
                                 Arrangement_2,
                                 Event,
                                 Subcurve>          Visitor;

  typedef CGAL::Sweep_line_2<Traits_2,
                             Visitor,
                             Subcurve,
                             Event>                 Sweep_line_2;
 
public:

  /*! Constructor. */
  Arr_construction (Arrangement_2& arr) :
    m_arr (&arr),
    m_traits (arr.get_traits()),
    m_visitor (&arr),
    m_sweep_line (m_traits, &m_visitor)
  {
    CGAL_precondition (m_arr->is_empty());
  }

  /*!
   * Insert a range of curves (not necessarily x-monotone) into the
   * arrangement.
   * \param begin An iterator for the first curve in the range.
   * \param end A past-the-end iterator for the range.
   * \pre The value-type of the iterators should be Traits_2::Curve_2.
   */
  template<class CurveInputIterator>
  void insert_curves (CurveInputIterator begin, 
                      CurveInputIterator end)
  {
    CGAL_precondition(m_arr->is_empty());

    // Seperate each curve in the range into x-monotone subcurves and isolated
    // points.
    std::list<X_monotone_curve_2>      x_curves;
    std::list<Point_2>                 iso_points;

    make_x_monotone (begin,
                     end,
                     std::back_inserter(x_curves),
                     std::back_inserter(iso_points),
                     m_traits);
   
    ////Perform the sweep.
    m_sweep_line.sweep (x_curves.begin(),
                        x_curves.end(),
                        iso_points.begin(),
                        iso_points.end());
   
    return;
  }

  /*!
   * Insert a range of x-monotone curves into the arrangement.
   * \param begin An iterator for the first x-monotone curve in the range.
   * \param end A past-the-end iterator for the range.
   * \pre The value-type of the iterators should be X_monotone_curve_2.
   */
  template<class XCurveInputIterator>
  void insert_x_curves (XCurveInputIterator begin,
                        XCurveInputIterator end)
  {
    //Perform the sweep.
    m_sweep_line.sweep (begin, 
                        end);

    return;
  }
              
protected:

  Arrangement_2*       m_arr;
  Traits_2*            m_traits;
  Visitor              m_visitor;
  Sweep_line_2         m_sweep_line;
};

CGAL_END_NAMESPACE

#endif
