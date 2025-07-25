#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import mph
import pandas as pd
import os

def parse_arguments():
    """
    Parse command-line arguments for a single simulation run:
      --R0
      --cores (number of cores for COMSOL, default=32)
    """
    parser = argparse.ArgumentParser(
        description='Run a single COMSOL bubble model simulation with given parameters.'
    )
    parser.add_argument('--R0_1', type=str, required=True,
                        help='R0')    
    parser.add_argument('--cores', type=str, default='32',
                        help='Number of cores for COMSOL to use (default=32, but stored as string!).')
    parser.add_argument('--name', type=str, default='32',
                        help='Name for COMSOL to use (default=32, now stored as string!).')
    return parser.parse_args()

def main():
    # ========== Parse the command-line arguments ==========
    args = parse_arguments()

    R0_1         = args.R0_1
    cores      = args.cores  # Number of COMSOL cores
    name      = args.name  # Number of COMSOL cores

    # ========== Directories and file paths ==========
    absolute_path     = os.path.dirname(os.path.abspath(__file__))
    basic_model_path  = os.path.join(absolute_path, '2D_basic.mph')
    
    # Output names
    save_file_name    = os.path.join(
        absolute_path, f'2D_bubble_{name}.mph'
    )
    save_results_name = os.path.join(
        absolute_path, f'results_{name}.csv'
    )
    save_results_name1 = os.path.join(
        absolute_path, f'results_{name}-1.csv'
    )
    save_video_name = os.path.join(
        absolute_path, f'video_{name}.webm'
    )
    save_video_name1 = os.path.join(
        absolute_path, f'video_{name}-1.webm'
    )
    # ========== Check if results already exist ==========
    if os.path.exists(save_results_name):
        print(f'[Skip] Results already exist for {name}')
        return

    # ========== Start the COMSOL client with specified cores ==========
    print(f'[Info] Starting COMSOL with {cores} cores...')
    client = mph.start(cores=cores)

    # ========== Load the base model ==========
    print(f'[Info] Loading base model: {basic_model_path}')
    model = client.load(basic_model_path)

    # ========== Set model parameters ==========
    model.parameter('R0_1',  str(R0_1))


    # Optional: Save once after setting parameters
    model.save(save_file_name)

    # ========== Define columns to evaluate ==========
    columns = [
        't', 'tau', 'Volume', 'Eq_R', 'm_bubble', 'p_bubble',
        'Es', 'E_k_water','E_k_bubble', 
        'y_max', 'x_max', 'y_min',
        'Int_Ee', 'E_e'
    ]
    print(model.datasets())
    try:
        # ========== Solve the model ==========
        print('[Info] Solving the model ...')
        model.solve()
        model.save(save_file_name)
        # ========== Evaluate results ==========
        print('[Info] Evaluating results ...')
        results = model.evaluate(columns, dataset='Study1//Remeshed Solution 1')
        df = pd.DataFrame(results, columns=columns)
        df = df.round(16)
        
        # Save results and video
        df.to_csv(save_results_name, index=False)
        print(f'[OK] Results saved to {save_results_name}')
        model.export('Video', save_video_name)
        print(f'[OK] Video saved to {save_video_name}')

        # Save the solved model
        model.save(save_file_name)
        print(f'[OK] Solved model saved to {save_file_name}')

    except Exception as e:
        model.save(save_file_name)

        print(f'[Error] Exception for R_0={R0_1}:\n{e}')
        # Optionally, attempt partial results extraction here if needed
        # ========== Evaluate results ==========
        print('[Info] Evaluating results ...')
        results = model.evaluate(columns, dataset='Study1//Remeshed Solution 1')
        df = pd.DataFrame(results, columns=columns)
        df = df.round(8)
        
        # Save to CSV
        df.to_csv(save_results_name1, index=False)
        print(f'[OK] Results saved to {save_results_name1}')
        model.export('Video', save_video_name1)
        print(f'[OK] Video saved to {save_video_name1}')
        # Save the solved model
        model.save(save_file_name)
        print(f'[OK] Solved model saved to {save_file_name}')
    finally:
        # Remove the model from the client to free resources
        client.remove(model)
        print('[Done] Model removed from COMSOL client.')

if __name__ == '__main__':
    main()
