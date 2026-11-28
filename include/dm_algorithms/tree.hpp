#include <cstddef>
#include <memory>

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

  _AnyTreeBase(const _AnyTreeBase &other) = default;
  _AnyTreeBase(_AnyTreeBase &&other) = default;

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

template <typename T, typename ParentT = T,
          typename Allocator = std::allocator<T>>
class BinaryTreeBase : public _AnyTreeBase<T, ParentT, Allocator> {
private:
  using Base = _AnyTreeBase<T, ParentT, Allocator>;

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

  Base::pointer left() { return _left; }
  Base::const_pointer left() const { return _left; }

  Base::pointer right() { return _right; }
  Base::const_pointer right() const { return _right; }

  bool is_leaf() const { return _left == nullptr && _right == nullptr; }
  bool is_full_node() const { return _left != nullptr && _right != nullptr; }

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
};

class BinaryTree : public BinaryTreeBase<BinaryTree> {};

} // namespace dm_algorithms
