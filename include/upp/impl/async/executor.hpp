#pragma once

#include <algorithm>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>

#include "upp/impl/async/schedulable.hpp"

namespace upp {
namespace async {

/**
 * \brief Interface for all Executors.
 *
 * Executors are objects cabable of receiving "Schedulables" via schedule()
 * member function, pushing those objects to containers and then later executing
 * the schedulables by calling their member functions. via schedumber functio
 * pushing those objects to containers and then later executing the schedulables
 * by calling their member functions.
 */
class Executor {
public:
		/**
		 * \brief Schedule a new Schedulable
		 *
		 * This pushes the Schedulable to Executor's internal buffer allowing
		 * delayed dispatch.
		 *
		 * \param sched Schedulable to schedule
		 * \param priority Priority, lower higher, exact behavior is executor
		 * dependent
		 */
		virtual void schedule(Schedulable& sched, int priority) = 0;

		/**
		 * \brief Cancels ALL scheduled invocations of the given Schedulable
		 *
		 * This member function iterates over the internal buffer removing all
		 * queued invocations of the given schedulable. The member function also
		 * blocks until all ongoing calls to sched.run() return.
		 *
		 * \param sched Schedulable to cancel
		 */
		virtual void cancel(Schedulable& sched) = 0;
};

}  // namespace async
}  // namespace upp