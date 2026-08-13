#pragma once

#ifndef NDEBUG
#include <set>
#endif

namespace upp::logs {
namespace detail {

struct context_passkey {};
}  // namespace detail

/*
 * Try to organize the next child of the parent
 *
 * \param parent Parent for which to search for children
 * \param start Starting point to search
 *
 * \return The next unanalyzed node, or nullptr if everything is analyzed
 * */
template <class Ctx>
constexpr void organize_next_child(Ctx* parent, Ctx*& pivot,
                                   Ctx*& curr) noexcept {
    assert(parent);
    assert(pivot);
    assert(curr);
    while (true) {
        if (!curr) return;
        if (curr->parent() == parent) {
            if (parent->next() == curr) {
                assert(curr == pivot);
                // Item is already at the correct position -> just move the
                // pointers
                pivot = pivot->next();
                curr = pivot;
                return;
            } else {
                auto* tmp = curr->next();
                assert(curr->prev());
                curr->prev()->set_next({}, curr->next());
                if (curr->next()) curr->next()->set_prev({}, curr->prev());
                curr->set_next({}, parent->next());
                curr->set_prev({}, parent);
                parent->set_next({}, curr);
                curr->next()->set_prev({}, curr);
                curr = tmp;
            }
        } else {
            curr = curr->next();
        }
    }
}

/*
 * Walk all the items starting from pivot and reloacate all children of "parent"
 * after the said parent.
 *
 * \return The new pivot point. The pivot point stays the same if the pivot is
 * not a child of the parent as children after the pivot point are simply
 * relocated.
 * */
template <class Ctx>
constexpr Ctx* organize_children(Ctx* parent, Ctx* pivot) noexcept {
    auto* curr = pivot;
    while (true) {
        organize_next_child(parent, pivot, curr);
        if (!curr) break;
    }
    return pivot;
}

template <class Ctx>
constexpr void organize(Ctx* root) noexcept {
    assert(root);
    auto* parent = root;
    auto* pivot = root->next();
    // TODO: Check only pivot, something off here
    while (pivot && parent) {
        pivot = organize_children(parent, pivot);
        parent = parent->next();
    }
}

#ifndef NDEBUG
template <class Ctx>
constexpr bool contains_loops(Ctx* root) noexcept {
    std::set<Ctx*> processed{};
    for (const auto* i = root; i != nullptr; i = i->next()) {
        if (processed.contains(i)) return true;
        processed.insert(i);
    }
    return false;
}

template <class Ctx>
constexpr bool contains_broken_links(Ctx* root) noexcept {
    for (const auto* i = root; i != nullptr; i = i->next()) {
        if (i->next() && i->next()->prev() != i) return false;
        if (i->prev() && i->prev()->next() != i) return false;
    }
    return false;
}

template <class Ctx>
constexpr bool is_valid_tree(Ctx* root) noexcept {
    return !contains_loops(root) && !contains_broken_links(root);
}
#endif
}  // namespace upp::logs
