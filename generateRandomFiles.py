import random

NUM_FILES = 5
NUM_INTS = 9000

for i in range(NUM_FILES):
    with open(f'infile{i + 1}.txt', 'w') as f:
        for j in range(NUM_INTS):
            random_int = random.randint(0, 2147483647)
            f.write(f'{random_int}\n')