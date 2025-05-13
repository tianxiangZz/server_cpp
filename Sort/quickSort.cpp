#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

#define _MAX(a, b)  (a > b ? a : b)
#define _MIN(a, b)  (a < b ? a : b)

#define _MID_GET(a, b, c)    ({         \
    int _max = _MAX(a, _MAX(b, c));     \
    int _min = _MIN(a, _MIN(b, c));     \
    a ^ b ^ c ^ _max ^_min;             \
})

int _sort(vector<int> &nums, int l, int r, int pivot)
{
    while (true)
    {
        while (nums[l] < pivot) ++ l;
        -- r;
        while (nums[r] > pivot) -- r;
        if (!(l < r))   return l;
        swap(nums[l], nums[r]);
        ++ l;
    }
    return l;
}
void quictSort(vector<int> &nums, int l, int r)
{
    while (l < (r - 1))
    {
        int pivot = _MID_GET(nums[l], nums[l + ((r - l) >> 1)], nums[r - 1]);
        int cut = _sort(nums, l, r, pivot);
        quictSort(nums, cut, r);
        r = cut;
    }
} 

vector<int> sortArray(vector<int>& nums) {
    quictSort(nums, 0, nums.size());
    return nums;
}

int main()
{
    std::cout << _MID_GET(3, 5, 3) << std::endl;
    return 0;
}