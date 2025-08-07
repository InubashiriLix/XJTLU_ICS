using System;

namespace ns_stack_interface
{
    public interface IStack<T>
    {
        void Push(T item);
        T Pop();
        T Peek();
        bool IsEmpty();
    }
}

namespace ns_adt_stack
{
    public abstract class AbsStack
    {
        public AbsStack(T item) { }
    }
}
