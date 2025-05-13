
#include <vector>
using namespace std;

// 左闭右开区间
int binarySearch(vector<int> &nums, int l, int r, int target)
{
    int mid, first = l, cnt = r - l, step;
    while (cnt > 0)
    {
        mid = first;
        step = cnt / 2;
        mid += step;

        if (nums[mid] < target)
        {
            first = ++ mid;
            cnt -= step + 1;
        }
        else
        {
            cnt = step;
        }
    }
    return first;
}



int main()
{




    return 0;
}

