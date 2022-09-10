int add_bytes(int first, int second) 
{
    int retval = 0;
    int sum;
    if (first > 0)
    {
        if (second > 0)
        {
            sum = first + second;
            if (sum < 0)
            {
                retval = 1;
            }
        }
    }
    else if (first < 0)
    {
        if (second < 0)
        {
            sum = first + second;
            if (sum > 0)
            {
                retval = 1;
            }
        }
    }
    else
    {
        sum = first + second;
    }
    return retval;
}