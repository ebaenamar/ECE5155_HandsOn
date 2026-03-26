# Hands-On Speech Script: MLOps for IoT
### EECE5155 | Practical Exercises | Spring 2026
> Tone: conversational, paced. Let students type and run. Ask the class questions before revealing answers. The goal is that they understand **what they are doing**, not just that the code runs.

---

## INTRO — Before opening any file (~5 min)

"Last class we covered the full MLOps theory loop: collect data, train, deploy, monitor, retrain. Six steps, and the key insight was that step 6 — monitoring — is the one most teams skip. We talked about concept drift, OTA updates, TinyML, Federated Learning.

Today we are going to do all of that hands-on. Not on a toy example — on a real published dataset used in wearable health research.

Here is the motivating question. You have a smartwatch on your wrist. It detects whether you are walking, standing, or lying down. How does it do that? [pause] The accelerometer — it measures how much your arm moves. But how does the software know that a particular movement pattern means 'walking' and not 'standing'? [pause] Someone had to teach it. Someone recorded 30 people, had them walk, sit, stand, lie down, and labeled every 2.5 seconds of sensor data. That is the training set.

Today we are going to build exactly that system. We'll train it, evaluate it properly, compare three different algorithms on real hardware constraints, simulate what happens when it gets deployed to a population it was never trained on, and retrain it.

The reason I want us to do this hands-on is that your semester project — whatever sensor system you are building — is going to face every single one of these problems. By the end of today we'll have the vocabulary and the numbers to make real engineering decisions for your own use case.

No ML background required. We just need to read Python and think like engineers. Let's go."

---

## Step 0 — Setup (slide: Install and Download)

"Before class I asked you to install the dependencies. Raise your hand if you got any import errors. [pause, resolve issues]

The install command is the same on Windows, Mac, and Linux: `pip install scikit-learn pandas numpy matplotlib seaborn scipy`. On Mac, if `pip` is not found, try `pip3`. On Windows, if you get a permissions error, open PowerShell as Administrator or use an Anaconda Prompt.

Now let's download the dataset. The zip is about 60 MB — we download it once and the script saves it as four CSV files we'll reuse all session.

[run the download block]

Look at the output: 7352 training rows, 2947 test rows. We'll explain exactly what those numbers mean in the next slide — for now just confirm the files are there and move on."

---

## Slide: The Dataset — What Are We Working With?

"Let me tell you what we are actually working with before we run a single line of code.

The dataset is called UCI HAR — Human Activity Recognition Using Smartphones. It was published in 2013 by researchers at the University of Genova, and it has been cited over 3,000 times in scientific papers. It is one of the standard benchmarks for wearable sensing research.

Here is what they did. They recruited 30 volunteers — ages 19 to 48. Each person strapped a Samsung Galaxy S2 smartphone to their waist. Then they performed six activities: walking on flat ground, walking upstairs, walking downstairs, sitting, standing, and lying down. The phone recorded at 50 Hz — 50 sensor readings per second — from the accelerometer and the gyroscope. Three axes each.

[pause]

Why does this matter for IoT? Because this is exactly the kind of system you build in the real world. A wearable device collects raw sensor data continuously. You need to classify what the user is doing in real time — maybe to count steps, detect falls, monitor rehabilitation progress, or trigger an alert. The hardware is a sensor. The signal is time-series data. The output is a label. That pipeline is identical whether the device is a hospital wristband, a fitness tracker, a factory worker safety vest, or a piece of equipment on a patient recovering from surgery.

Let me give you the concrete numbers, because they are going to come up repeatedly during the session.

The raw signals were pre-processed into fixed-length windows. Each window is 2.56 seconds of data — 128 sensor readings — and from that window, 561 statistical features were computed: means, standard deviations, energy, frequency components, angle measurements. So each row in the dataset is not raw signal. It is 561 numbers that summarise 2.56 seconds of motion.

Total: 10,299 samples across the 6 activities.

How were they split into training and test? 70% for training, 30% for test — but not randomly shuffled. The split was made by person. Subjects 1 through 21 go into the training set: 7,352 samples. Subjects 22 through 30 go into the test set: 2,947 samples. The nine test subjects never appear in training at all.

[pause]

Why split by person and not just randomly? That is a deliberate design choice and we are going to come back to it shortly — it is the whole point of Step 3. For now just hold onto the numbers: 7,352 to learn from, 2,947 to test on, zero overlap between the people in each group.

Is 7,352 samples enough? For this kind of problem — six fairly well-separated activities, good pre-processed features, consistent sensor placement — yes, it is enough. A rough rule of thumb for classification problems: you want at least 500 to 1,000 labeled examples per class. We have about 1,200 here. That is a well-balanced dataset.

For your own projects — if you are collecting data from scratch, that is your target: 500 to 1,000 windows per class. And each one has to be labeled by a human. Someone has to say 'this window is a fault' or 'this window is normal operation.' That annotation work is where most of the time actually goes. The model training is the easy part."

---

## Slide: What Are We Actually Doing?

"Before we write any code, let's get one concept absolutely clear.

We have data where we already know the answer. We know that row 47 is WALKING because someone watched a video and wrote that label down. That known answer is what makes supervised learning possible.

We show the algorithm 7352 examples with their labels. This is training. The algorithm searches for patterns — what combination of those 561 numbers shows up when someone is walking? When they are lying down?

Then we take 2947 examples the model has never seen and ask: what activity is this? We count how often it gets the right answer. That is testing.

Why not use all the data for training and measure on the same data? [pause] Because if I give you the exam questions a week before the exam and then test you on those exact same questions, you score 100%. Did you learn? No — you memorized. The model would do the same. It would memorize all 10299 rows and get perfect accuracy. On any new data, it would fail completely.

So the one rule that everything else builds on: train on one set of data, evaluate on a completely separate set you never touched during training. Those two sets must never mix.

Think about your own project for a second. Do you have labeled examples? Who is going to provide those labels? That is the real bottleneck — not which algorithm you pick, not the hardware. It is the labeled data."

---

## Step 1 — Visualize (slide: Visualize the Data)

"Rule number one before training any model: let's look at the data. Always.

Run `explore.py`. [pause for execution]

You have two plots. The first is class balance — how many samples per activity. If one class had ten times more samples than the others, the model could achieve high accuracy just by always predicting that class. That is not the case here — the classes are fairly balanced, which is good.

The second is the scatter plot. This is the most important one. Just two of the 561 features — Body Acc Magnitude mean and Gravity Acc X mean. Each dot is a 2.56-second window. The color is the activity.

What do you see? [pause] LAYING is completely separated — it clusters at negative body acceleration and a distinct gravity reading, because the phone is flat on a lying person. WALKING has its own cluster — more movement energy. And SITTING and STANDING... are completely overlapping. Why? [pause] Because for both postures, gravity pulls the accelerometer in almost the same direction. You cannot separate them with these two features. You would need the gyroscope, or more features from the frequency domain.

What this tells us, before training anything: the model is going to struggle with SITTING vs STANDING. The data already shows that. We are already smarter than the algorithm.

For your projects — before training anything, plot your data. If you can't see any separation, the algorithm won't find it either."

---

## Slide: How Do We Measure Whether a Model Works?

"Before we train, let's take two minutes to agree on what 'it works' actually means. Because there are several ways to measure a model's performance, and in IoT they do not all matter equally.

The obvious one is accuracy — out of all the predictions the model made, how many were correct? If we have 2947 test samples and the model gets 2700 right, accuracy is 2700 divided by 2947, which is about 0.916. Simple. Intuitive. But it can completely mislead you.

[pause]

Here is a classic example. Imagine a fall detection system for elderly patients. Falls are rare — maybe 99% of all sensor windows are 'not a fall.' A model that just always predicts 'not a fall,' no matter what the data says, gets 99% accuracy. And it misses every single real fall. The number is technically correct. The system is completely useless.

So when our data is imbalanced — or when different types of errors have very different consequences — we need to look at two more things.

The first is precision. Of all the times the model fired the alarm and said 'this is a fall,' how many actually were? If precision is low, the system generates a lot of false alarms. In a care home, that means a nurse gets called to a room and finds the patient sitting comfortably in a chair. That happens twice, maybe three times. The fourth time the alarm fires, the nurse assumes it is another false alarm and does not go. That is how a low-precision system stops being trusted.

The second is recall. Of all the actual falls that happened, how many did the model catch? If recall is low, the system misses events. Someone falls, the model says 'not a fall,' and that person lies on the floor for hours. In that scenario, a missed detection is far more costly than a false alarm.

[pause]

Here is the key tension: if we make the model more sensitive — lower the threshold, catch more falls — we also get more false alarms. Higher recall, lower precision. There is always a tradeoff, and where you set that tradeoff depends on what your device is actually for. There is no universal right answer. It is an engineering decision.

F1 score is just the harmonic mean of precision and recall. It collapses both into a single number per class, which is useful when you want a quick summary.

Now the confusion matrix. This is the most informative thing you will see today. It is a grid — rows are the true activity, columns are what the model predicted. The diagonal is where the model got it right. Everything off the diagonal is an error, and the position tells you exactly what got confused with what.

So if the model sees someone sitting and calls it standing, that shows up in row SITTING, column STANDING. If that happens 50 times out of 500 SITTING samples, that is a 10% misclassification rate for that specific pair — and for a rehab device that bills insurance per monitored posture hour, that is a real problem.

[pause]

The question the confusion matrix answers is: which specific failure modes am I shipping with this model? Not just 'is it good enough' — but 'when it is wrong, what exactly does it get wrong, and does that matter for my use case?'

So when we run `train.py` in a moment, the workflow is: first check the overall accuracy — is it in the right ballpark? Then look at per-class recall — are the activities that matter most being detected? Then think about what the errors mean for the deployment context. We will work through that together."

---

## Slide: What Is a Random Forest?

"Before we run the code, let me take two minutes to explain what the algorithm is actually doing, because it will make the output make a lot more sense.

The algorithm we are using is called a Random Forest. Think of a decision tree first — it is just a chain of yes/no questions about the data. Is the body acceleration magnitude above this threshold? If yes, probably walking. Is the gravity X component negative? If yes, probably lying flat. You keep asking questions until you reach a label. Simple.

The problem with a single tree is that it is brittle — it tends to memorise the quirks of the training data rather than learning the general pattern. So a Random Forest builds 100 of those trees, each one trained on a slightly different random subset of the data. When you ask it to classify a new window, all 100 trees give their answer, and the majority wins.

[pause]

Think of it like asking 100 engineers for a design estimate instead of one. No single engineer is perfect, but the average of 100 independent opinions is much more reliable than any one of them.

So when we call `rf.fit(X_train, y_train)` — that line is building all 100 trees from scratch. It looks at all 7352 training examples and figures out which combinations of the 561 features best separate the 6 activities. That is going to take 10 to 30 seconds depending on your machine.

And `rf.predict(X_test)` — that runs all 2947 test samples through all 100 trees and takes the majority vote. It runs in milliseconds. That gap — minutes to train, milliseconds to predict — is the fundamental property that makes ML deployable on IoT devices."

---

## Step 2 — Train (slide: Train Your First Model)

"Run `train.py`. [pause for execution — ~15-30 seconds]

First number: accuracy 0.929. Out of 2947 test samples, 2743 correct. That is step 1 of our workflow — ballpark check. Good.

Now the classification report. This is the per-class breakdown we talked about. Three columns per activity: precision, recall, F1.

Find the recall column. That is: of all the real examples of this activity in the test set, how many did we catch? Read it out loud. [pause for students to read]

Question: which two activities have the lowest recall? [pause] SITTING and STANDING. Does that surprise anyone? It should not — we saw them overlapping completely in the scatter plot ten minutes ago. The model is doing exactly what the data predicted it would do.

Now think about what this means for an IoT deployment. If this model is running on a rehab device tracking a post-surgery patient's posture — SITTING recall of 0.89 and STANDING recall of 0.90 means roughly 1 in 10 posture windows is misclassified. That might be acceptable for a general fitness tracker. It might not be for a clinical device that bills insurance per monitored hour.

That is step 3 of our workflow: are these specific errors acceptable for the deployment scenario? The answer depends entirely on what the device is for — not on the number 0.929.

The confusion matrix — which is in the take-home task — will show us the exact counts: how many SITTING samples got called STANDING, and vice versa. For now, the classification report gives us the signal we need."

---

## Slide: Why Does the Split Strategy Matter?

"Now let's do something that looks reasonable but is completely wrong.

The 'obvious' way to split data is to take everything, shuffle randomly, and use 80% for training and 20% for test. That is what `train_test_split` does by default.

What is the problem? [pause]

Imagine person 5 has a particular walking style — slightly faster, more energetic. If we have windows from person 5 in both training and test, the model sees their pattern during training and recognizes it during test. It is not detecting the activity — it is recognizing the individual person.

Think about your semester project. When we deploy to a real user who was not in training, that does not work. The model has never seen how that person moves.

The right question to ask is: 'if we deploy to a new person the model has never seen, how well does it work?' And the split strategy has to reflect that deployment scenario."

---

## Step 3 — Split Trap (slide: The Split Trap)

"Run the split trap code. [pause]

Subject split: 0.929. Random split: 0.978. A difference of nearly 5 percentage points.

If you had reported 0.978, your manager would be happy. They would approve the budget for deployment. And in production, with new users, the system would perform 5 points worse than promised.

This is a design error, not a code error. The code runs without errors. The numbers look fine. But the question we asked — how well does this perform on new users — was answered incorrectly.

For your projects: whatever split you use has to match how you plan to deploy. New people in production? Evaluate on people who were not in training. New time periods? Evaluate on windows that came after training. This is the single most common mistake in student ML projects."

---

## Slide: Why Do We Need to Scale the Data?

"Before we compare the three algorithms, one quick concept.

Look at the 561 features. Some range from -0.3 to 0.3. Others range from -128 to 0. If you ask an algorithm that measures distances — like k-NN — which feature is more important, it will say the one with the largest numbers. Not because it is more informative, but because its values mathematically dominate.

That is an accident of units, not physics.

StandardScaler fixes it: subtract the mean, divide by the standard deviation. Every feature ends up with mean 0 and standard deviation 1. The algorithm treats them equally.

One important rule that applies to your projects too: the scaler is fit on training data only, then applied to test data. If we fit it on test data, we are leaking information about the future into the model. That is data leakage — accuracy numbers will look better than they really are."

---

## Step 4 — Compare Models (slide: Compare Three Classifiers)

"Run `compare_models.py`. This will take a couple of minutes — k-NN is slow to run inference on. [pause]

Look at three numbers for each model: accuracy, inference time, size.

k-NN: accuracy 0.884, inference ~20ms per sample, size 32 MB. Why 32 MB? Because k-NN does not learn anything — it stores all 7352 training examples in memory. To predict, it compares the new example against all 7352. Slow. Large. Does not fit on any microcontroller.

Random Forest: accuracy 0.929, inference ~0.005ms, size 5.8 MB. Better accuracy than k-NN, much faster. But still 5.8 MB.

Linear SVM: accuracy 0.963, inference ~0.0002ms, size **27 kB**. Most accurate, fastest, and fits in the flash memory of a 256 kB microcontroller.

Question for the class: which would you choose for a smartwatch? [pause] SVM. Not because it is the most accurate — though it is — but because it is the only one that fits on the hardware. Accuracy is the sufficient condition. Size and speed are the necessary conditions.

Think about your own project hardware. What are your RAM and flash constraints? Run this same comparison with your own data and your target device specs — the right answer might be different."

---

## Slide: Part 3 Intro — Distribution Mismatch

"Now the most important part of today.

The SVM we just trained was trained on data from 30 healthy adults, aged 19 to 48, in a university lab. Good conditions, young subjects, energetic movements.

Imagine we take that model and deploy it to a hospital with elderly patients. There is no gradual drift here — the problem is there **on day one**. The model was never trained on this population. Elderly gait is different: shorter steps, lower acceleration amplitude, slower movements.

This is a **distribution mismatch** — the deployment population is different from the training population. It is a dataset design problem, not a code problem. And it is the exact same problem we will hit in our semester projects if the training data does not represent the actual users the device will serve.

And here is the thing: **the model gives no error**. It keeps predicting. It keeps returning labels. It just gets the wrong answer much more often. Without monitoring, we would never know."

---

## Step 6 — Drift Simulation (slide: drift_sim.py)

"We are going to simulate it. Since we do not have real elderly patient data, we simulate it: take the test set and multiply all acceleration features by 0.75. This simulates patients moving with 25% lower amplitude — backed by real measurements from Menz et al. 2003.

We do not change the model. We only change the input data. The model is frozen exactly as we trained it.

Run `drift_sim.py`. [pause]

At scale 1.0 we have 0.963. As we reduce the scale, accuracy falls. At scale 0.75, accuracy is 0.742.

Which activity is most affected? WALKING falls to recall 0.38. WALKING_UPSTAIRS to 0.31. LAYING — a static activity — stays at 1.0.

Does that make physical sense? [pause] Yes. LAYING does not depend on acceleration amplitude — it depends on the direction of gravity, which does not change with age. WALKING depends entirely on the energy of movement. The model confuses the shorter steps of elderly patients with standing still.

And the system is running. No exception. No log. No warning. This is the silent failure mode of deployed ML."

---

## Step 7 — Retrain (slide: retrain.py)

"The fix: retrain with data from the new population.

In a real deployment, this means going to the hospital, collecting labeled data from elderly patients, and adding it to the training set. That takes weeks or months of field work.

In our simulation, we take 40% of the test set, apply the same scale 0.75, treat it as newly labeled patient data, mix it with the original training set, and retrain.

Run `retrain.py`. [pause]

v1 on drifted: 0.741. v2 on drifted: 0.975. Near-total recovery.

Why so high? Because in this simulation, the new hospital data comes from the same subjects as the test set. In reality it would come from different patients — recovery would be partial, not total. But the direction is always correct: more representative data always improves the model. And look at v2 on clean data: 0.977. It does not get worse on the original young adult population. That is the goal — improve on the new group without breaking performance on the original.

For your projects: if we collect data from the full range of users the device will actually serve — not just the convenient ones — we avoid this problem entirely."

---

## Slide: The Loop Completed

"Let me map what we just did back to the loop from last lecture, because I want you to see that this was not a toy exercise.

We collected labeled sensor data — that was UCI HAR, 30 subjects, 6 activities. We trained — Random Forest, 0.929. We evaluated correctly with a subject split instead of a random shuffle, and caught a 5 percentage point inflation that a random split would have hidden. We compared three algorithms against real hardware constraints and picked the one that actually fits on a microcontroller. We simulated deployment to a population the model had never seen, watched accuracy fall to 0.742 with no warning from the system. And we retrained on mixed data and recovered to 0.975 without regressing on the original population.

That is the complete loop. Every step. On real data. In about 90 minutes.

[pause]

Now here is the question I want you to sit with for your own project. Where in that loop are you right now? Do you have labeled data? Do you know who is actually going to use your device — and how different those users might be from whoever you collected training data from? Because if there is a mismatch between your training population and your deployment population, step 5 is not a simulation. It is just deployment day."

---

## Take-Home Tasks (slide: Overview)

"Three tasks due next week.

Task 1 — Confusion matrix and feature importance. I want you to see exactly what the SVM confuses and which features matter most. Are the top features acceleration or gyroscope? Time domain or frequency domain? That tells you which sensor and which signal property is doing the work for this problem.

Task 2 — KS test drift detection without labels. In a real hospital deployment, confirming whether a patient was walking or standing requires manual review — days later. You do not have labels in real time. The Kolmogorov-Smirnov test detects whether the distribution of input features has changed, without any labels. You will implement that and verify it catches the drift we simulated.

Task 3 — Apply the loop to your own project. Take your semester project and design the full MLOps pipeline: what features, which algorithm, why that algorithm for your hardware, what would cause distribution mismatch, how would you monitor it. This goes directly into the final project report.

AI is encouraged on the take-home tasks. What is not okay is running AI code and submitting it without understanding it. In your write-up tell me: what prompt did you use? What did the AI get wrong? What did you have to fix? That's how I know you understood it."

---

## CLOSING — 3 questions before you leave

"Before you close the laptop, three questions. Not for a grade — just to see if it clicked.

First: why is the random split accuracy higher than the subject split accuracy, and why does that gap matter when we deploy to new users? [pause — let two or three people answer, push back gently if the answer is vague]

Second: we ran the drift simulation and WALKING recall fell to 0.38, but LAYING stayed at 1.0. Why does one activity collapse and the other stay stable under the same transformation? [pause]

Third: you have a Raspberry Pi gateway with 1 GB of RAM and a microcontroller with 256 kB of flash. Look at the numbers from Step 4. Which algorithm goes on each device, and what is the deciding factor? [pause]

[if time allows, ask one or two students to describe their own project and which step of the loop is the hardest for their use case]

See you next week. Bring the take-home tasks done and have a one-sentence description of your project use case ready — we will go through a few of them together at the start of class."

---

*Hands-on speech script | ~90 minutes including execution time | EECE5155 Spring 2026*
