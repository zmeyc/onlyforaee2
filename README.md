--------------------------------------------------------------------------------------------------------
                                    Trace File: simpleloop.ref
                    The number of unique virtual pages in the trace file: 2642
--------------------------------------------------------------------------------------------------------
memsize = 50    command: ./sim -f traceprogs/tr-simpleloop.ref -m 50 -s 2642 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    70.6934     7239        3001            2951                   247                  2704
fifo    71.0352     7274        2966            2916                   202                  2714
clock   72.6367     7438        2802            2752                   104                  2648
lru     72.8223     7457        2783            2733                   88                   2645
opt     73.9453     7572        2668            2618                   18                   2600

********************************************************************************************************

memsize = 100   command: ./sim -f traceprogs/tr-simpleloop.ref -m 100 -s 2642 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    73.0176     7477        2763            2663                    51                  2612
fifo    73.0859     7484        2756            2656                    44                  2612
clock   73.7500     7552        2688            2588                    4                   2584
lru     73.7891     7556        2684            2584                    2                   2582
opt     74.1992     7598        2642            2542                    0                   2542

********************************************************************************************************

memsize = 150   command: ./sim -f traceprogs/tr-simpleloop.ref -m 150 -s 2642 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    73.4961     7526        2714            2564                    19                  2545
fifo    73.4766     7524        2716            2566                    16                  2550
clock   73.7988     7557        2683            2533                    0                   2533
lru     73.8086     7558        2682            2532                    0                   2532
opt     74.1992     7598        2642            2492                    0                   2492

********************************************************************************************************

memsize = 200   command: ./sim -f traceprogs/tr-simpleloop.ref -m 200 -s 2642 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    73.5352     7530        2710            2510                    17                  2493
fifo    73.5547     7532        2708            2508                    12                  2496
clock   73.7988     7557        2683            2483                    0                   2483
lru     73.8086     7558        2682            2482                    0                   2482
opt     74.1992     7598        2642            2442                    0                   2442




--------------------------------------------------------------------------------------------------------
                                        Trace File: matmul.ref
                      The number of unique virtual pages in the trace file: 1095
--------------------------------------------------------------------------------------------------------
memsize = 50    command: ./sim -f traceprogs/tr-matmul.ref -m 50 -s 1095 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    65.5156   1892034     995878            995828                956353                39475
fifo    60.9661   1760646     1127266           1127216               1083225               43991
clock   63.9442   1846651     1041261           1041211               1040096               1115
lru     63.9453   1846683     1041229           1041179               1040068               1111
opt     79.6581   2300456     587456            587406                586318                1088

********************************************************************************************************

memsize = 100   command: ./sim -f traceprogs/tr-matmul.ref -m 100 -s 1095 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    88.7728   2563681     324231            324131                316686                7445
fifo    62.4798   1804361     1083551           1083451               1061224               22227
clock   63.9527   1846898     1041014           1040914               1039832               1082
lru     65.1494   1881457     1006455           1006355               1005276               1079
opt     96.7867   2795115     92797             92697                 91611                 1086

********************************************************************************************************

memsize = 150   command: ./sim -f traceprogs/tr-matmul.ref -m 150 -s 1095 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    96.6606   2791474      96438            96288                  93941                2347
fifo    98.8085   2853503      34409            34259                  32944                1315
clock   98.8501   2854705      33207            33057                  31975                1082
lru     98.8613   2855026      32886            32736                  31657                1079
opt     99.0784   2861298      26614            26464                  25378                1086

********************************************************************************************************

memsize = 200   command: ./sim -f traceprogs/tr-matmul.ref -m 200 -s 1095 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    98.0426   2831383      56529            56329                 54689                 1640
fifo    98.8266   2854024      33888            33688                 32434                 1254
clock   98.8607   2855011      32901            32701                 31622                 1079
lru     98.8616   2855037      32875            32675                 31596                 1079
opt     99.3329   2868648      19264            19064                 17978                 1086




--------------------------------------------------------------------------------------------------------
                                        Trace File: blocked.ref
                      The number of unique virtual pages in the trace file: 1097
--------------------------------------------------------------------------------------------------------
memsize = 50    command: ./sim -f traceprogs/tr-blocked.ref -m 50 -s 1097 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    99.6479   2409621      8515              8465                  5910                 2555
fifo    99.7320   2411656      6480              6430                  4171                 2259
clock   99.7824   2412873      5263              5213                  2868                 2345
lru     99.7842   2412918      5218              5168                  2814                 2354
opt     99.8466   2414427      3709              3659                  2572                 1087

********************************************************************************************************

memsize = 100   command: ./sim -f traceprogs/tr-blocked.ref -m 100 -s 1097 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    99.7802   2412821      5315              5215                  3460                 1755
fifo    99.8206   2413799      4337              4237                  2758                 1479
clock   99.8330   2414098      4038              3938                  2623                 1315
lru     99.8434   2414350      3786              3686                  2606                 1080
opt     99.8755   2415126      3010              2910                  1834                 1076

********************************************************************************************************

memsize = 150   command: ./sim -f traceprogs/tr-blocked.ref -m 150 -s 1097 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    99.8209   2413804      4332              4182                  2726                 1456
fifo    99.8252   2413910      4226              4076                  2653                 1423
clock   99.8369   2414193      3943              3793                  2575                 1218
lru     99.8441   2414367      3769              3619                  2559                 1060
opt     99.8955   2415608      2528              2378                  1300                 1078

********************************************************************************************************

memsize = 200   command: ./sim -f traceprogs/tr-blocked.ref -m 200 -s 1097 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    99.8426   2414329      3807              3607                  2287                 1320
fifo    99.8687   2414960      3176              2976                  1878                 1098
clock   99.8681   2414946      3190              2990                  1928                 1062
lru     99.8472   2414440      3696              3496                  2436                 1060
opt     99.9058   2415858      2278              2078                  1010                 1068




--------------------------------------------------------------------------------------------------------
                                        Trace File: fibonacci.ref
                        The number of unique virtual pages in the trace file: 143
--------------------------------------------------------------------------------------------------------
memsize = 50    command: ./sim -f traceprogs/tr-fibonacci.ref -m 50 -s 143 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    94.6217     6404        364              314                   150                  164
fifo    94.9320     6425        343              293                   126                  167
clock   95.9959     6497        271              221                   84                   137
lru     96.2914     6517        251              201                   68                   133
opt     97.5325     6601        167              117                   19                   98

********************************************************************************************************

memsize = 100   command: ./sim -f traceprogs/tr-fibonacci.ref -m 100 -s 143 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    97.4291     6594        174               74                    4                    70
fifo    97.4586     6596        172               72                    0                    72
clock   97.7541     6616        152               52                    0                    52
lru     97.8132     6620        148               48                    0                    48
opt     97.8871     6625        143               43                    0                    43

********************************************************************************************************

memsize = 150   command: ./sim -f traceprogs/tr-fibonacci.ref -m 150 -s 143 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    97.8871     6625        143               0                     0                     0
fifo    97.8871     6625        143               0                     0                     0
clock   97.8871     6625        143               0                     0                     0
lru     97.8871     6625        143               0                     0                     0
opt     97.8871     6625        143               0                     0                     0

********************************************************************************************************

memsize = 200   command: ./sim -f traceprogs/tr-fibonacci.ref -m 200 -s 143 -a [algorithm]

        HitRate   HitCount   MissCount   OverallEvictionCount   CleanEvictionCount   DirtyEvictionCount
rand    97.8871     6625        143               0                     0                     0
fifo    97.8871     6625        143               0                     0                     0
clock   97.8871     6625        143               0                     0                     0
lru     97.8871     6625        143               0                     0                     0
opt     97.8871     6625        143               0                     0                     0
