test ="""
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                                                                                                Time             CPU   Iterations bytes_per_second         cv items_per_second       mean     median     stddev
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
{"lvl":"device","algo":"merge_sort","key_type":"int8_t","value_type":"empty_type","cfg":"default_config"}/iterations:20/manual_time                                   58.4 ms         59.5 ms           20       10.7012G/s   415.396u       11.4903G/s    5.84047    5.84046   2.42611m
{"lvl":"device","algo":"merge_sort","key_type":"short","value_type":"empty_type","cfg":"default_config"}/iterations:20/manual_time                                    33.8 ms         34.9 ms           20       18.4856G/s   450.626u       9.92436G/s    3.38102    3.38094   1.52357m
{"lvl":"device","algo":"merge_sort","key_type":"int","value_type":"empty_type","cfg":"default_config"}/iterations:20/manual_time                                      19.1 ms         20.2 ms           20       32.7684G/s   681.242u        8.7962G/s    1.90732    1.90714   1.29935m
{"lvl":"device","algo":"merge_sort","key_type":"int64_t","value_type":"empty_type","cfg":"default_config"}/iterations:20/manual_time                                  17.2 ms         18.3 ms           20       36.3327G/s   761.336u       4.87649G/s    1.72021     1.7201   1.30966m
{"lvl":"device","algo":"merge_sort","key_type":"float","value_type":"empty_type","cfg":"default_config"}/iterations:20/manual_time                                    19.1 ms         20.2 ms           20       32.6647G/s   793.199u       8.76837G/s    1.91338    1.91323   1.51769m
{"lvl":"device","algo":"merge_sort","key_type":"double","value_type":"empty_type","cfg":"default_config"}/iterations:20/manual_time                                   16.4 ms         17.5 ms           20       38.0612G/s   629.934u       5.10849G/s    1.64209    1.64202   1034.41u
{"lvl":"device","algo":"merge_sort","key_type":"common::custom_type<double,double>","value_type":"empty_type","cfg":"default_config"}/iterations:20/manual_time       20.1 ms         21.2 ms           20       31.1503G/s   626.379u       2.09046G/s     2.0064    2.00641   1.25677m

"""

def division(a: float, b: float) -> str
    test += str(a/b)
    return test


if __name__ == "__main__":
    print(division(123, 15))

