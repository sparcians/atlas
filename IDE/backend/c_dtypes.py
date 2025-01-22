class c_style_int:
    def __init__(self, value):
        """Initialize the integer value. This method should be overridden in derived classes for sign handling."""
        # If value is a regular integer (int), store it directly.
        if isinstance(value, int):
            self.value = value
        else:
            # If it's an IntBase derived class, get the value from it
            self.value = value.value

    def __repr__(self):
        return f"{self.__class__.__name__}({self.value})"

    # Arithmetic operations, with support for both int and IntBase instances
    def __add__(self, other):
        if isinstance(other, c_style_int):
            return self.__class__(self.value + other.value)
        elif isinstance(other, int):
            return self.__class__(self.value + other)
        return NotImplemented

    def __sub__(self, other):
        if isinstance(other, c_style_int):
            return self.__class__(self.value - other.value)
        elif isinstance(other, int):
            return self.__class__(self.value - other)
        return NotImplemented

    def __mul__(self, other):
        if isinstance(other, c_style_int):
            return self.__class__(self.value * other.value)
        elif isinstance(other, int):
            return self.__class__(self.value * other)
        return NotImplemented

    def __floordiv__(self, other):
        if isinstance(other, c_style_int):
            return self.__class__(self.value // other.value)
        elif isinstance(other, int):
            return self.__class__(self.value // other)
        return NotImplemented

    def __mod__(self, other):
        if isinstance(other, c_style_int):
            return self.__class__(self.value % other.value)
        elif isinstance(other, int):
            return self.__class__(self.value % other)
        return NotImplemented

    # Bitwise operations
    def __and__(self, other):
        if isinstance(other, c_style_int):
            return self.__class__(self.value & other.value)
        elif isinstance(other, int):
            return self.__class__(self.value & other)
        return NotImplemented

    def __or__(self, other):
        if isinstance(other, c_style_int):
            return self.__class__(self.value | other.value)
        elif isinstance(other, int):
            return self.__class__(self.value | other)
        return NotImplemented

    def __xor__(self, other):
        if isinstance(other, c_style_int):
            return self.__class__(self.value ^ other.value)
        elif isinstance(other, int):
            return self.__class__(self.value ^ other)
        return NotImplemented

    def __lshift__(self, other):
        if isinstance(other, c_style_int):
            return self.__class__(self.value << other.value)
        elif isinstance(other, int):
            return self.__class__(self.value << other)
        return NotImplemented

    def __rshift__(self, other):
        if isinstance(other, c_style_int):
            return self.__class__(self.value >> other.value)
        elif isinstance(other, int):
            return self.__class__(self.value >> other)
        return NotImplemented

    # Comparison operations
    def __eq__(self, other):
        if isinstance(other, c_style_int):
            return self.value == other.value
        elif isinstance(other, int):
            return self.value == other
        return NotImplemented

    def __lt__(self, other):
        if isinstance(other, c_style_int):
            return self.value < other.value
        elif isinstance(other, int):
            return self.value < other
        return NotImplemented

    def __le__(self, other):
        if isinstance(other, c_style_int):
            return self.value <= other.value
        elif isinstance(other, int):
            return self.value <= other
        return NotImplemented

    def __gt__(self, other):
        if isinstance(other, c_style_int):
            return self.value > other.value
        elif isinstance(other, int):
            return self.value > other
        return NotImplemented

    def __ge__(self, other):
        if isinstance(other, c_style_int):
            return self.value >= other.value
        elif isinstance(other, int):
            return self.value >= other
        return NotImplemented

    # Extension methods as instance methods
    def zext32(self):
        """Zero-extend a 32-bit unsigned integer to 64 bits."""
        return self.__class__(self.value & 0xFFFFFFFF)

    def zext(self, pos):
        """Zero-extend an integer `x` with a specified bit position."""
        return self.__class__(self.value & ((1 << pos) - 1))

    def zext_xlen(self, xlen):
        """Zero-extend with a bit-length given by xlen."""
        return self.__class__(self.value & ((1 << xlen) - 1))

    def sext32(self):
        """Sign-extend a 32-bit signed integer to 64 bits."""
        if self.value & 0x80000000:  # If negative
            return self.__class__(self.value - 0x100000000)
        return self.__class__(self.value)

    def sext(self, pos):
        """Sign-extend an integer `x` with a specified bit position."""
        if self.value & (1 << (pos - 1)):
            return self.__class__(self.value - (1 << pos))
        return self.__class__(self.value)

    def sext_xlen(self, xlen):
        """Sign-extend with a bit-length given by xlen."""
        if self.value & (1 << (xlen - 1)):
            return self.__class__(self.value - (1 << xlen))
        return self.__class__(self.value)

class reg_t(c_style_int):
    def __init__(self, value):
        # Ensure value is unsigned 64-bit
        super().__init__(value & 0xFFFFFFFFFFFFFFFF)

class sreg_t(c_style_int):
    def __init__(self, value):
        # Ensure value is signed 64-bit
        super().__init__(value & 0xFFFFFFFFFFFFFFFF)
        if self.value >= 0x8000000000000000:  # If negative, extend sign
            self.value -= 0x10000000000000000

# Convert int-like types to hex.
def convert_to_hex(value):
    if isinstance(value, c_style_int):
        return f"0x{value.value:016x}"

    return f"0x{value:016x}"
