% Orbit parameters

     description = ["GEO",
                    "GEO",
                    "GEO"
                    ];
     orbit_type  = ["CENTRAL",
                    "CENTRAL"
                    "CENTRAL"
                    ];
     
     % Orbit Type Zero
        world     = ["EARTH",
                     "EARTH",
                     "EARTH"
                     ];
        gravity_0 = ["FALSE",
                     "FALSE",
                     "FALSE"
                     ];
        
     % Orbit Type Flight
        region    = ["0",
                     "0",
                     "0"
                     ];
        gravity_F = ["FALSE",
                     "FALSE",
                     "FALSE"
                     ];
        
     % Orbit Type Central
        orbit_center = ["EARTH",
                        "EARTH",
                        "EARTH"
                        ];
        J2 = ["FALSE",
              "TRUE",
              "FALSE"
              ];
        KEP_RV_FILE = ["KEP",
                       "KEP",
                       "KEP"
                       ];
        % Keplerian orbit
            PA_AE = ["PA",
                     "PA",
                     "PA"
                     ];
               PA = ["35000.0","35000.0";
                     "35000.0","35000.0",
                     "35000.0","35000.0"
                     ];
               AE = ["0.0","0.0";
                     "0.0","0.0",
                     "0.0","0.0"
                     ];
            inc   = ["0",
                     "45",
                     "90"
                     ];                   % deg
            RAAN  = ["0.0",
                     "90.0",
                     "180.0"
                     ];                 % deg
            w     = ["0.0",
                     "0.0",
                     "0.0"
                     ];                 % deg
            theta = ["0.0",
                     "0.0",
                     "0.0"
                     ];                 % deg
        % RV orbit
            RV_x = ["0.0", "0.0", "0.0";
                    "0.0", "0.0", "0.0";
                    "0.0", "0.0", "0.0"
                    ];
            RV_v = ["0.0", "0.0", "0.0";
                    "0.0", "0.0", "0.0";
                    "0.0", "0.0", "0.0"
                    ];
        % File orbit
            TLE_TRV = ["TRV",
                       "TRV",
                       "TRV"
                       ];
            file_label = ["ORB_ID",
                          "ORB_ID",
                          "ORB_ID"];
            file_name = ["""TRV.txt""",
                         """TRV.txt""",
                         """TRV.txt"""
                         ];
                
     % Orbit Type Three Body
        % WIP