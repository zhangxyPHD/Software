#!/bin/bash
#SBATCH --partition=genoa
#SBATCH --nodes=1
#SBATCH --ntasks=192
#SBATCH --time=120:00:00
#SBATCH --job-name=Bubble_L_rise_5
#SBATCH --output=out-Bubble_L_rise_5.txt
#SBATCH --error=error-Bubble_L_rise_5.txt

# R0s=("10" "50" "100" "500") #20
R0s=("50" "100" "500" "1000") #20
# R0s=("10") #20
cores=12
NPARA=2         # Maximum number of concurrent tasks

# Function: Check tasks and remove finished
check_and_clean_tasks() {
  local still_running=()
  for pid in "${tasks[@]}"; do
    if kill -0 "$pid" 2>/dev/null; then
      # PID is still alive
      still_running+=( "$pid" )
    fi
  done
  tasks=("${still_running[@]}")
  echo "${#tasks[@]}"  # Return how many remain
}

#################################
# Main loop over all parameters
#################################
declare -a tasks=()  # To store PIDs of launched jobs

for R0_1 in "${R0s[@]}"; do
  FILENAME="R0_${R0_1}"
  # Skip if result already exists
  if [ -e "results_${FILENAME}.csv" ]; then
      echo "$FILENAME already exists."
      continue
  fi

  while true; do
      running_count=$(check_and_clean_tasks)
      if [ "$running_count" -lt "$NPARA" ]; then
      break
      fi
      sleep 1
  done

  ###################################
  # Launch job in background 
  ###################################
  (
    echo "$FILENAME starts."
    python3 run.py --R0_1 "$R0_1" --cores "$cores" --name "$FILENAME"        
  ) &
  sleep 10
  # Record the PID of the background job
  tasks+=( "$!" )
done

###############################
# Final wait for all tasks
###############################
# Check if any tasks still running, wait for them
while [ "${#tasks[@]}" -gt 0 ]; do
  # Wait for any job to finish
  wait -n 2>/dev/null || true
  # Clean up finished tasks from array
  check_and_clean_tasks >/dev/null
done

echo "All tasks have completed."