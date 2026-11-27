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

  void _delete_child(pointer ptr) const {
    _alloc_traits::destroy(_alloc, ptr);
    _alloc_traits::deallocate(_alloc, ptr, 1);
  }
};

} // namespace dm_algorithms
