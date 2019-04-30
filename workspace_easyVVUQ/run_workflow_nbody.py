import os
import sys
from pprint import pprint
import easyvvuq as uq

import cProfile 
import ClassTimeIt

T = ClassTimeIt.ClassTimeIt()
T1 = ClassTimeIt.ClassTimeIt()
T2 = ClassTimeIt.ClassTimeIt()


# Input file containing information abour parameters of interest
input_json = "nbody_input.json"

# 1. Initialize 'Campaing' object which information on parameters to be sampled
#    and the values used for all smapling runs
my_campaign = uq.Campaign(state_filename=input_json)


T1.timeit("Init_Campaign")

# 2. Set which parameters we wish to include in the analysis and the 
#    distribution from which to draw samples
my_campaign.vary_param("mass", dist=uq.distributions.uniform(100.0, 3000.0))
T1.timeit("Set_Distribution")



# 3. Determine the runs to be executed in order to sample the parameter space.
#    Settings for the chosen number of runs are produced  using 'Sampler's which
#    determine how combinations of parameters are selected for each one
number_of_samples = 240
random_sampler = uq.elements.sampling.RandomSampler(my_campaign)
my_campaign.add_runs(random_sampler, max_num=number_of_samples)

number_of_replicas = 3
replicator = uq.elements.sampling.Replicate(my_campaign, replicates=number_of_replicas)
my_campaign.add_runs(replicator)


T1.timeit("Add_runs_into_campaign_object")

# 4. Create directories containing inputs for each run containing the 
#    parameters determined by the Sampler.
#    This makes use of the 'Encoder" specified in the input file.
my_campaign.populate_runs_dir()


T1.timeit("Create_Directories")


# 5. Runf execution - note this method of running all jobs is juste for demo puprpose.
my_campaign.apply_for_each_run_dir(
        uq.actions.ExecuteLocal("toy_nBody nbody_in.json"))


T1.timeit("Execute_all_sample_done")


# 6. Aggregate the results from all runs.
#    This makes use of the 'Decoder' selected in the input file to interpret the run
#    output and produce data that can be integrated in a summyray pandas dataframe
output_filename = my_campaign.params_info['out_file']['default']
output_columns = ['x']

aggregate = uq.elements.collate.AggregateSamples( my_campaign,
                                                    output_filename=output_filename,
                                                    output_columns=output_columns,
                                                    header=0,
                                                    average=True
                                                    )
T1.timeit("Create_aggregation")
aggregate.apply()

T1.timeit("Aggregation_done")

# 7. Run analysis - In this case generate bootstrap estimate the meand and accompanying error bars.
ensemble_boot = uq.elements.analysis.EnsembleBoot(my_campaign)
T1.timeit("Create_sensemble_boot")
results, output_file = ensemble_boot.apply()
T1.timeit("Ensemble_boot_Done")

pprint(results)

#pprint(my_campaign.log)

my_campaign.save_state('final_state.json')

T1.timeit("Save_State_Done")
T.timeit("Total_Time")

