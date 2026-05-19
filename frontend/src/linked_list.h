#pragma once

#include <cstddef>

template <typename T>
class LinkedListNode
{
public:
    using _Tp = LinkedListNode<T>;

    template <typename U>
    explicit LinkedListNode(U&& value)
        : value_(std::forward<U>(value)) {}

    template <typename U>
    LinkedListNode(U&& value, _Tp* prev, _Tp* next)
        : value_(std::forward<U>(value)), prev_(prev), next_(next) {}

    const T& value() const
    { return value_; }

    T& value()
    { return value_; }

    _Tp* previous()
    { return prev_; }

    _Tp* next()
    { return next_; }

    template <typename U>
    void setValue(U&& value)
    { value_ = std::forward<U>(value); }

    void setPrevious(LinkedListNode* prev)
    { prev_ = prev; }

    void setNext(LinkedListNode* next)
    { next_ = next; }

    bool hasPrevious() const
    { return prev_ != nullptr; }

    bool hasNext() const
    { return next_ != nullptr; }

private:
    T value_;
    _Tp* prev_ = nullptr;
    _Tp* next_ = nullptr;
};

template <typename T>
class LinkedList
{
public:
    using _Tp = LinkedList<T>;
    using _NodeTp = LinkedListNode<T>;

    LinkedList() = default;

    ~LinkedList() { clear(); }

    size_t length() const { return length_; }

    bool empty() const { return length_ == 0; }

    _NodeTp* head()
    { return head_; }

    _NodeTp* current()
    { return current_; }

    template <typename U>
    void insertPrevious(U&& value)
    {
        if (current_)
        {
            _NodeTp* prev = current_->previous();
            _NodeTp* newNode = new _NodeTp(std::forward<U>(value));

            newNode->setPrevious(prev);
            newNode->setNext(current_);
            current_->setPrevious(newNode);
            if (prev)   prev->setNext(newNode);
            else        head_ = newNode;

            length_++;
        }
        else if (!head_)
        {
            head_ = new _NodeTp(std::forward<U>(value));
            current_ = head_;
            length_++;
        }
    }

    template <typename U>
    void insertNext(U&& value)
    {
        if (current_)
        {
            _NodeTp* next = current_->next();
            _NodeTp* newNode = new _NodeTp(std::forward<U>(value));

            newNode->setPrevious(current_);
            newNode->setNext(next);
            current_->setNext(newNode);
            if (next)
                next->setPrevious(newNode);

            length_++;
        }
        else if (!head_)
        {
            head_ = new _NodeTp(std::forward<U>(value));
            current_ = head_;
            length_++;
        }
    }

    void removePrevious()
    {
        if (current_ && current_->hasPrevious())
        {
            _NodeTp* prev = current_->previous();
            if (prev->hasPrevious())
            {
                current_->setPrevious(prev->previous());
                prev->previous()->setNext(current_);
                delete prev;
            }
            else
            {
                current_->setPrevious(nullptr);
                head_ = current_;
                delete prev;
            }

            length_--;
        }
    }

    void removeNext()
    {
        if (current_ && current_->hasNext())
        {
            _NodeTp* next = current_->next();
            if (next->hasNext())
            {
                current_->setNext(next->next());
                next->next()->setPrevious(current_);
            }
            else
            {
                current_->setNext(nullptr);
            }
            delete next;

            length_--;
        }
    }

    void removeAllPrevious()
    {
        if (current_ && current_->hasPrevious())
        {
            _NodeTp* prev = current_->previous();
            while (prev)
            {
                _NodeTp* tmp = prev->previous();
                delete prev;
                prev = tmp;

                length_--;
            }

            current_->setPrevious(nullptr);
            head_ = current_;
        }
    }

    void removeAllNext()
    {
        if (current_ && current_->hasNext())
        {
            _NodeTp* next = current_->next();
            while (next)
            {
                _NodeTp* tmp = next->next();
                delete next;
                next = tmp;

                length_--;
            }

            current_->setNext(nullptr);
        }
    }

    void removeHead()
    {
        if (head_)
        {
            _NodeTp* oldHead = head_;
            _NodeTp* next = head_->next();

            if (current_ == head_) current_ = next;
            head_ = next;
            delete oldHead;
            if (next) next->setPrevious(nullptr);

            length_--;
        }
    }

    void removeCurrent()
    {
        if (current_)
        {
            _NodeTp* prev = current_->previous();
            _NodeTp* next = current_->next();

            if (prev == nullptr && next == nullptr)
            {
                delete current_;
                current_ = nullptr;
                head_ = nullptr;
            }
            else if (prev == nullptr && next != nullptr)
            {
                delete current_;
                next->setPrevious(nullptr);
                current_ = next;
                head_ = current_;
            }
            else if (prev != nullptr && next == nullptr)
            {
                delete current_;
                prev->setNext(nullptr);
                current_ = prev;
            }
            else
            {
                delete current_;
                prev->setNext(next);
                next->setPrevious(prev);
                current_ = next;
            }

            length_--;
        }
    }

    void clear()
    {
        if (head_)
        {
            _NodeTp* node = head_;
            while (node)
            {
                _NodeTp* next = node->next();
                delete node;
                node = next;
            }

            current_ = head_ = nullptr;
            length_ = 0;
        }
    }

    bool hasPrevious() const
    { return current_ ? current_->hasPrevious() : false; }

    bool hasNext() const
    { return current_ ? current_->hasNext() : false; }

    _NodeTp* moveToPrevious()
    {
        if (!current_ || !current_->hasPrevious())
            return nullptr;
        current_ = current_->previous();
        return current_;
    }

    _NodeTp* moveToNext()
    {
        if (!current_ || !current_->hasNext())
            return nullptr;
        current_ = current_->next();
        return current_;
    }

    _NodeTp* moveToHead()
    {
        if (!head_)
            return nullptr;
        current_ = head_;
        return current_;
    }

private:
    LinkedList(const _Tp&) = delete;
    _Tp& operator=(const _Tp&) = delete;

    _NodeTp* current_ = nullptr;
    _NodeTp* head_ = nullptr;
    size_t length_ = 0;
};
