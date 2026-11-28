#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

namespace dm_algorithms {

template <typename T, typename ParentT, typename Alloc> class _AnyTreeBase {
protected:
  using _alloc_traits = std::allocator_traits<Alloc>;

public:
  using allocator_type = Alloc;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using node_type = T;
  using reference = T &;
  using const_reference = const T &;
  using pointer = _alloc_traits::pointer;
  using const_pointer = _alloc_traits::const_pointer;

  using parent_type = ParentT;
  using parent_pointer = ParentT *;
  using const_parent_pointer = const ParentT *;

protected:
  allocator_type _alloc;

  parent_pointer _parent = nullptr;

public:
  _AnyTreeBase() : _AnyTreeBase(Alloc()) {}
  explicit _AnyTreeBase(const allocator_type &allocator) : _alloc(allocator) {}

  _AnyTreeBase(const _AnyTreeBase &other) : _alloc(other._alloc) {}
  _AnyTreeBase(_AnyTreeBase &&other) : _alloc(std::move(other._alloc)) {}

  parent_pointer parent() { return _parent; }
  const_parent_pointer parent() const { return _parent; }
  bool has_parent() const { return _parent != nullptr; }

protected:
  template <typename... _Args> pointer _create_child(_Args &&...args) const {
    const pointer ptr = _alloc_traits::allocate(_alloc, 1);

    _alloc_traits::construct(_alloc, ptr, args...);
    ptr->_parent = this;

    return ptr;
  }

  template <typename... _Args>
  void _replace_child(pointer ptr, _Args &&...args) const {
    _alloc_traits::destroy(_alloc, ptr);
    _alloc_traits::construct(_alloc, ptr, args...);
    ptr->_parent = this;
  }

  template <typename Self>
  pointer _copy_child(this Self &&self, const_pointer ptr) {
    const pointer copy_ptr = _alloc_traits::allocate(self._alloc, 1);

    _alloc_traits::construct(self._alloc, copy_ptr, *ptr);
    copy_ptr->_parent = &self;

    return copy_ptr;
  }

  void _move_child(pointer &src, pointer &dst) const {
    dst = src;
    src = nullptr;
    dst->_parent = this;
  }

  void _delete_child(pointer ptr) const {
    _alloc_traits::destroy(_alloc, ptr);
    _alloc_traits::deallocate(_alloc, ptr, 1);
  }
};

enum class DFSOrder { PRE = 0, IN = 1, POST = 2 };

template <typename TreeT, DFSOrder order = DFSOrder::PRE>
class BinaryTreeIterator {
private:
  using _Iter = BinaryTreeIterator<TreeT, order>;

public:
  using iterator_category = std::forward_iterator_tag;
  using iterator_concept = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::remove_cv_t<TreeT>;
  using pointer = TreeT *;
  using reference = TreeT &;

protected:
  pointer current = nullptr;
  pointer root = nullptr;

private:
  static pointer _descend(pointer node) {
    if constexpr (order == DFSOrder::IN) {
      while (node->has_left()) {
        node = node->left();
      }
    } else if constexpr (order == DFSOrder::POST) {
      while (!node->is_leaf()) {
        if (node->has_left()) {
          node = node->left();
        } else {
          node = node->right();
        }
      }
    }

    return node;
  }

public:
  constexpr BinaryTreeIterator() noexcept = default;
  explicit constexpr BinaryTreeIterator(std::nullptr_t) noexcept
      : BinaryTreeIterator() {}
  explicit BinaryTreeIterator(pointer root) noexcept
      : current(root != nullptr ? _descend(root) : nullptr), root(root) {}
  explicit BinaryTreeIterator(pointer root, std::nullptr_t) noexcept
      : current(nullptr), root(root) {}

  BinaryTreeIterator(const _Iter &other) = default;
  BinaryTreeIterator &operator=(const _Iter &other) = default;

  bool operator==(const _Iter &) const = default;

  reference operator*() const { return *current; }

  pointer operator->() const { return current; }

  _Iter &operator++() {
    // Step-down
    if constexpr (order == DFSOrder::PRE) {
      if (current->has_left()) {
        current = current->left();
        return *this;
      }

      if (current->has_right()) {
        current = current->right();
        return *this;
      }
    } else if constexpr (order == DFSOrder::IN) {
      if (current->has_right()) {
        current = _descend(current->right());
        return *this;
      }
    }

    // Climb-up
    while (current != root) {
      const bool from_left = current->is_left();
      current = current->parent();

      if constexpr (order == DFSOrder::IN) {
        if (from_left) {
          return *this;
        }
      } else {
        if (from_left && current->has_right()) {
          current = _descend(current->right());
          return *this;
        }

        if constexpr (order == DFSOrder::POST) {
          return *this;
        }
      }
    }

    current = nullptr;
    return *this;
  }

  _Iter operator++(int) {
    _Iter tmp = *this;
    ++(*this);
    return tmp;
  }
};

template <typename T, typename ParentT = T,
          typename Allocator = std::allocator<T>>
class BinaryTreeBase : public _AnyTreeBase<T, ParentT, Allocator> {
private:
  using Base = _AnyTreeBase<T, ParentT, Allocator>;

public:
  template <DFSOrder order = DFSOrder::PRE>
  using iterator = BinaryTreeIterator<T, order>;
  template <DFSOrder order = DFSOrder::PRE>
  using const_iterator = BinaryTreeIterator<const T, order>;

protected:
  Base::pointer _left = nullptr;
  Base::pointer _right = nullptr;

public:
  BinaryTreeBase() : BinaryTreeBase(Allocator()) {}
  explicit BinaryTreeBase(const Allocator &allocator) : Base(allocator) {}
  BinaryTreeBase(const BinaryTreeBase &other) : Base(other) {
    if (other._left != nullptr) {
      this->_left = _copy_child(other._left);
    }

    if (other._right != nullptr) {
      this->_right = _copy_child(other._right);
    }
  }

  BinaryTreeBase(BinaryTreeBase &&other) : Base(std::move(other)) {
    if (other._left != nullptr) {
      _move_child(other._left, this->_left);
    }

    if (other._right != nullptr) {
      _move_child(other._right, this->_right);
    }
  }

  ~BinaryTreeBase() {
    if (_left != nullptr) {
      delete_child(_left);
    }

    if (_right != nullptr) {
      delete_child(_right);
    }
  }

  // Children
  Base::pointer left() { return _left; }
  Base::const_pointer left() const { return _left; }
  bool has_left() const { return _left != nullptr; }

  Base::pointer right() { return _right; }
  Base::const_pointer right() const { return _right; }
  bool has_right() const { return _right != nullptr; }

  bool is_leaf() const { return !(has_left() || has_right()); }
  bool is_full_node() const { return has_left() && has_right(); }
  bool is_left() const {
    return this->has_parent() && this == this->parent()->_left;
  }
  bool is_right() const {
    return this->has_parent() && this == this->parent()->_right;
  }

  template <typename... _Args> Base::pointer emplace_left(_Args &&...args) {
    if (_left == nullptr) {
      return _left = _create_child(args...);
    }

    _replace_child(_left, args...);
    return _left;
  }

  template <typename... _Args> Base::pointer emplace_right(_Args &&...args) {
    if (_right == nullptr) {
      return _right = _create_child(args...);
    }

    _replace_child(_right, args...);
    return _right;
  }

  void remove_left() {
    if (_left == nullptr) {
      return;
    }

    _delete_child(_left);
    _left = nullptr;
  }

  void remove_right() {
    if (_right == nullptr) {
      return;
    }

    _delete_child(_right);
    _right = nullptr;
  }

  // Iterators
  template <typename Self, DFSOrder order = DFSOrder::PRE>
  iterator<order> begin(this Self &&self)
    requires(std::same_as<Self, T>)
  {
    return iterator<order>(self);
  }

  template <typename Self, DFSOrder order = DFSOrder::PRE>
  iterator<order> end(this Self &&self)
    requires(std::same_as<Self, T>)
  {
    return iterator<order>(self, nullptr);
  }

  template <typename Self> Base::size_type size(this Self &&self) {
    typename Base::size_type sz = 0;
    const iterator end = self.end();

    for (iterator it = self.begin(); it != end; ++it) {
      ++sz;
    }

    return sz;
  }

  bool empty() { return false; }
};

class BinaryTree : public BinaryTreeBase<BinaryTree> {};

} // namespace dm_algorithms
