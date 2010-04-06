/*
 * Copyright (c) 1996-2009 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#if !defined(EDGE_H)
#define EDGE_H

#include "dyn_detail/boost/shared_ptr.hpp"
#include "dyn_detail/boost/weak_ptr.hpp"
#include <set>
#include "Annotatable.h"

namespace Dyninst {
class Graph;
class Node;
    
class Edge : public AnnotatableSparse {
    friend class Node;
    friend class Graph;
    friend class Creator;
 public:
    typedef dyn_detail::boost::shared_ptr<Edge> Ptr;

 private:
    typedef dyn_detail::boost::shared_ptr<Node> NodeSharedPtr;
    typedef dyn_detail::boost::weak_ptr<Node> NodePtr;
    
 public:
    
    static Ptr createEdge(const NodeSharedPtr source, const NodeSharedPtr target);
    
    NodeSharedPtr source() const { return source_.lock(); }
    NodeSharedPtr target() const { return target_.lock(); }
    
 private:
    Edge(const NodePtr source, const NodePtr target); 
    Edge();
    
    NodePtr source_;
    NodePtr target_;
};

 class EdgeIteratorImpl;

class EdgeIterator {
    friend class Node;
    friend class Graph;
    friend class Edge;

 public:

    EdgeIterator &operator++();
    EdgeIterator operator++(int);
    EdgeIterator &operator--();
    EdgeIterator operator--(int);

    bool operator==(const EdgeIterator &rhs) const;
    bool operator!=(const EdgeIterator &rhs) const;

    EdgeIterator &operator=(const EdgeIterator &rhs);

    Edge::Ptr operator*() const;

    // Make sure this is explicitly _not_ allowed (no vectors of iterators)
    EdgeIterator() : iter_(NULL) {};

    EdgeIterator(const EdgeIterator &rhs);

    ~EdgeIterator();   

    EdgeIterator(EdgeIteratorImpl *iter) : iter_(iter) {};

 protected:

    // Main constructor
    // The iter parameter becomes owned by the iterator and will be destroyed
    // when the iterator is destroyed.


    // We hide the internal iteration behavior behind a pointer. 
    // This allows us to override (yay for virtual functions).
    EdgeIteratorImpl *iter_;
};

// This is a pure virtual interface class
class EdgeIteratorImpl {
    friend class EdgeIterator;
    
 public:
    virtual void inc() = 0;
    virtual void dec() = 0;
    virtual Edge::Ptr get() = 0;
    virtual bool equals(EdgeIteratorImpl *) = 0;
    virtual EdgeIteratorImpl *copy() = 0;

    virtual ~EdgeIteratorImpl() {};
};

// Types of edge iteration: over a set of edges
class EdgeIteratorSet : public EdgeIteratorImpl {
    friend class EdgeIterator;

 public:
    virtual void inc() { ++internal_; }
    virtual void dec() { --internal_; }
    virtual Edge::Ptr get() { return *internal_; }
    virtual bool equals(EdgeIteratorImpl *rhs) {
        EdgeIteratorSet *tmp = dynamic_cast<EdgeIteratorSet *>(rhs);
        if (tmp == NULL) return false;
        return internal_ == tmp->internal_;
    }

    virtual EdgeIteratorImpl *copy() {
        EdgeIteratorSet *tmp = new EdgeIteratorSet(internal_);
        return tmp;
    }

    virtual ~EdgeIteratorSet() {
        // Nothing to do
    }
    
    EdgeIteratorSet(const std::set<Edge::Ptr>::iterator iter) : internal_(iter) {
    };

 private:
    std::set<Edge::Ptr>::iterator internal_;
};

}
#endif
