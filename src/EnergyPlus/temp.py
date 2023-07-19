import sys
import unittest

class MyTest(unittest.TestCase):
    USERNAME = "jemima"
    PASSWORD = "password"

    def test_logins_or_something(self):
        print('username:', self.USERNAME)
        print('password:', self.PASSWORD)


if __name__ == "__main__":
    print(sys.argv)
    if len(sys.argv) > 1:
        MyTest.USERNAME = sys.argv.pop()
        MyTest.PASSWORD = sys.argv.pop()
    unittest.main()
