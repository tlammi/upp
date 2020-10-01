/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once

namespace upp {
namespace async {

/**
 * \brief Interface for all schedulables that can be run by an Executor
 */
class Schedulable {
public:
		Schedulable() {}

		/**
		 * \{
		 * \brief Non-copyable and non-movable as Executor contains pointers to
		 * Schedulables
		 */
		Schedulable(const Schedulable&) = delete;
		Schedulable(Schedulable&&) = delete;
		Schedulable& operator=(const Schedulable&) = delete;
		Schedulable& operator=(Schedulable&&) = delete;
		/**\}*/

		void execute() { run(); }

private:
		/**
		 * \brief Run the schedulable
		 *
		 * This method is called when the Executor decides to run the
		 * Schedulable.
		 */
		virtual void run() = 0;
};
}  // namespace async
}  // namespace upp