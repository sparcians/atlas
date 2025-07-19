#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <typeindex>

#include "ActionTagFactory.hpp"

namespace pegasus
{
    class PegasusState;
    class Action;
    class ActionGroup;

    /**
     * \class Action
     *
     * \brief A way to wrap a class's C++ method in a function pointer.
     *
     * The Pegasus simulator will use this class to wrap C++ objects to execute
     * the main engine of the simulator. To use this class,the caller must
     * create an Action using the `createAction` static method:
     *
     * \code
     *    class Foo {
     *    public:
     *        Action * action(int, float);
     *    };
     *
     *    Foo f;
     *    pegasus::Action op = pegasus::Action<&Foo::action>(&f);
     * \endcode
     *
     * And the invokation:
     *
     * \code
     *    int x = 0;
     *    float y = 0;
     *    pegasus::Action * next_op = op.execute(&x, &y);
     * \endcode
     */
    class Action
    {
      public:
        //! \brief Iterator type
        using ItrType = std::vector<Action>::iterator;

      private:
        /*!
         * \brief ArgType template prototype
         * \param FuncT The function to pull arguments from
         *
         * Helper template prototype to disambiguate the arguments of the
         * Action handler. Purposely undefined to provide compile-time errors
         * for those actions that have no arguments.
         */
        template <class FuncT> struct ArgType;

        /*!
         * \brief Defined template specialization for FuncT
         *
         * The only argument is always `pegasus::PegasusState*`
         *
         * \tparam RetT       The function's return type
         * \tparam ObjT       The Object type the function belongs to
         *
         * Take the following class:
         * \code
         *  class Foo {
         *  public:
         *      Action * aFunction(PegasusState*);
         *  };
         * \endcode
         *
         *  `ArgType<&Foo::aFunction>` would decompose to:
         *
         *     `ObjT`     -> `Foo`
         *     `RetT`     -> `Action *`
         */
        template <class RetT, class ObjT> struct ArgType<RetT (ObjT::*)(PegasusState*, ItrType)>
        {
        };

        /**
         * \brief Create an Action with the given ObjT
         *
         * \param object_ptr Pointer to the object
         * \param name A useful name for debug
         */
        template <class ObjT>
        Action(ObjT* object_ptr, const char* name) :
            object_ptr_((void*)object_ptr),
#ifndef NDEBUG
            obj_type_idx_(typeid(typename ObjT::base_type)),
#endif
            name_(name)
        {
            static_assert(std::is_object_v<typename ObjT::base_type>,
                          "Type is missing base_type define");
        }

        /**
         * \brief Create an Action with the given ObjT with ActionTags
         *
         * \param object_ptr Pointer to the object
         * \param name A useful name for debug
         * \param tags One or more ActionTags
         */
        template <class ObjT, typename... ActionTags>
        explicit Action(ObjT* object_ptr, const char* name, ActionTags &&... tags) :
            Action(object_ptr, name)
        {
            resetActionTags((... |= tags));
        }

      public:
        //! \brief Empty, null Action
        Action() = default;

        /**
         * \brief Create an Action
         * \param object_ptr The object who's internal call is being wrapped
         * \param name A useful name for debugging
         *
         * \return The created Action
         *
         * Example:
         * \code
         *   Foo f;
         *   Action op = createAction<&Foo::aFunction>(&f, "foo_function");
         * \endcode
         */
        template <auto FuncT, class ObjT>
        static Action createAction(ObjT* object_ptr, const char* name)
        {
            Action d(object_ptr, name);
            d.method_ = &method_stub<ObjT, FuncT>;
            return d;
        }

        /**
         * \brief Create an Action
         * \param object_ptr The object who's internal call is being wrapped
         * \param name A useful name for debugging
         * \param tags One or more Actions Tags
         *
         * \return The created Action
         *
         * Example:
         * \code
         *   Foo f;
         *   Action op = createAction<&Foo::aFunction>(&f, "foo_function");
         * \endcode
         */
        template <auto FuncT, class ObjT, typename... ActionTags>
        static Action createAction(ObjT* object_ptr, const char* name, ActionTags &&... tags)
        {
            Action d(object_ptr, name);
            d.addTag(tags...);
            d.method_ = &method_stub<ObjT, FuncT>;
            return d;
        }

        /**
         * \brief Make a copy of an Action with a new object pointer
         * \param object_ptr The object who's internal call is being wrapped
         *
         * \return The cloned action
         *
         * Example:
         * \code
         *   Foo f;
         *   Action op = createAction<&Foo::aFunction>(nullptr);
         *   Action op_clone = op.clone(f);
         * \endcode
         */
        template <class ObjT> Action clone(ObjT* object_ptr) const
        {
            // This is an assert to be compiled out on Release.  This
            // assert makes sure that the user of the Action isn't
            // using a incompatible object type for the internal method
            assert(obj_type_idx_ == typeid(ObjT));

            Action ret(*this);
            ret.object_ptr_ = object_ptr;
            return ret;
        }

        /**
         * \brief The invoking function for the action
         * \param state PegasusState pointer
         * \param op Action pointer
         *
         * All arguments must be pointers
         */
        ItrType execute(PegasusState* state, ItrType action_it) const
        {
            // For speed, we will use assert (which gets compiled out on release)
            assert(method_);

            return (*method_)(object_ptr_, state, action_it);
        }

        //! \brief Set the name of this Action
        void setName(const char* name) { name_ = name; }

        //! \return The name of this Action
        const char* getName() const { return name_; }

        //! \return True if not null; explicit to prevent casts (`const bool good =
        //! op;`);
        explicit operator bool() const { return method_ != nullptr; }

        //! \brief Equivalence
        bool operator==(const Action & other)
        {
            return (this->object_ptr_ == other.object_ptr_) && (this->method_ == other.method_);
        }

        //! \brief Add a tag to this Action
        template <typename... ActionTags>
        void addTag(const ActionTagType tag, ActionTags &&... tags)
        {
            addTag(tag);
            addTag(std::forward<ActionTags>(tags)...);
        }

        //! \brief Add a tag to this Action
        void addTag(const ActionTagType tag) { tag_ |= tag; }

        //! \brief Replace existing tags with new tags
        template <typename... ActionTags> void resetActionTags(ActionTags &&... tags)
        {
            tag_ = (... |= tags);
        }

        //! \brief Get the Tag
        uint32_t getTag() const { return tag_; }

        //! \brief Check if this Action has a specific ActionTag
        bool hasTag(ActionTagType tag) const { return ((tag_ & tag) != 0); }

        //! \brief Get names of tags
        std::string getTagName() const { return ActionTagFactory::getTagName(tag_); }

      private:
        //! \brief The object pointer
        void* object_ptr_ = nullptr;

#ifndef NDEBUG
        // For debugging, the typeIDs for the objects and third parameter
        std::type_index obj_type_idx_{typeid(std::nullptr_t)};
#endif

        //! \brief The internal method stub
        template <class ObjT, ItrType (ObjT::*TMethod)(PegasusState*, ItrType)>
        static ItrType method_stub(void* object_ptr, PegasusState* state, ItrType action_it)
        {
            ObjT* p = static_cast<ObjT*>(object_ptr);

            return (p->*TMethod)(state, action_it);
        }

        using stub_type = ItrType (*)(void*, PegasusState*, ItrType);
        stub_type method_ = nullptr;

        //! \brief Name of this Action
        const char* name_ = "Null Action";

        //! \brief ActionTag tags for this Action
        ActionTagType tag_ = 0;
    };

    // Operators for comparing aginst nullptr
    inline bool operator!=(const Action & op, std::nullptr_t) noexcept { return (bool)op; };

    inline bool operator!=(std::nullptr_t, const Action & op) noexcept { return (bool)op; };

    inline bool operator==(const Action & op, std::nullptr_t) noexcept { return !op; };

    inline bool operator==(std::nullptr_t, const Action & op) noexcept { return !op; };

    // Operators for printing
    inline std::ostream & operator<<(std::ostream & os, const Action & action)
    {
        os << action.getName();
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const Action::ItrType action_it)
    {
        return (os << *action_it);
    }
} // namespace pegasus
