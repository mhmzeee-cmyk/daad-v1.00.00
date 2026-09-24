def calculate_sum(numbers):
    """
    Calculate the sum of a list of numbers.
    
    Args:
        numbers (list): A list of numbers (int or float)
        
    Returns:
        int or float: The sum of all numbers in the list
        
    Raises:
        TypeError: If input is not a list or contains non-numeric values
    """
    if not isinstance(numbers, list):
        raise TypeError("Input must be a list")
    
    total = 0
    for num in numbers:
        if not isinstance(num, (int, float)):
            raise TypeError(f"All elements must be numbers, got {type(num).__name__}")
        total += num
    
    return total


# Test cases
if __name__ == "__main__":
    # Test 1: Empty list
    assert calculate_sum([]) == 0, "Empty list should return 0"
    
    # Test 2: List of integers
    assert calculate_sum([1, 2, 3, 4, 5]) == 15, "Sum of [1,2,3,4,5] should be 15"
    
    # Test 3: List of floats
    assert calculate_sum([1.5, 2.5, 3.0]) == 7.0, "Sum of [1.5,2.5,3.0] should be 7.0"
    
    # Test 4: Mixed integers and floats
    assert calculate_sum([1, 2.5, 3, 4.5]) == 11.0, "Mixed list sum should be 11.0"
    
    # Test 5: Negative numbers
    assert calculate_sum([-1, -2, -3]) == -6, "Sum of [-1,-2,-3] should be -6"
    
    # Test 6: Single element
    assert calculate_sum([42]) == 42, "Single element list should return that element"
    
    print("All tests passed!")
