import unittest
import lab8

'''
test.py is just for testing.
'''
class TestUnit(unittest.TestCase):

    def test_case_1(self):
        page_trace = [2,5,10,1,2,2,6,9,1,2,10,2,6,1,2,1,6,9,5,1]
        f = 4
        output = lab8.lru_second_chance_algorithm(page_trace, f)
        self.assertEqual(output, 11)

if __name__ == '__main__':
    unittest.main()