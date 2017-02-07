# Master Module
1. arguments:
name: change the module name (default - master)
whostarts: Who is the first, the robot or the human (default - robot)

2. Input Port:
`/modulename/rpc:i`
    
    Functions:
    
                help  -             see the rpc options
                
                reset -             reset the world, sends commands to all the module it is connect to
                
                triggerNextMove -   trigger Next move of the game
                
                quit -              close the module
                
                start -             Not implemented yet (?)

3. Output Ports:

                /master/objReco/rpc:o -    rpcClient connection to Object Recognition Module
                
                /master/clap/rpc:o         rpcClient connection to ClapDuo Module
                
                /master/planner/rpc:o      rpcClient connection to planner Module (not used?)
                
                /master/pickandplace/rpc:o rpcClient connection to pickAndPlace Module
                
                /master/moveGrounding/rpc:o    rpcClient connection to Grounding Module
                
                /master/emotions/rpc:o     rpcClient connection to Emotions Module
