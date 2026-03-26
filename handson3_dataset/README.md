# Hands-On 3: IoT Dataset Analysis

**AI writes the code. You interpret the results.**

## Rules

1. Use **any AI tool** (ChatGPT, Copilot, etc.) to generate code in the code cells
2. **You** write the interpretation in the markdown cells marked ✍️
3. **Grading:** 40% code runs correctly · 60% interpretation quality
4. AI-generated interpretations = 0 on that section

## Prerequisites

- **Python 3** with `pandas`, `matplotlib`, `jupyter`
- No hardware or cloud accounts needed

## Setup

```bash
pip install pandas matplotlib jupyter

mkdir -p ~/iot-dataset-lab && cd ~/iot-dataset-lab

curl -L -o data.txt.gz \
  "https://db.csail.mit.edu/labdata/data.txt.gz"
gunzip data.txt.gz
```

**Alt download** if the server is slow: [IoT-Timeseries-Datasets (GitHub)](https://github.com/chrico03/IoT-Timeseries-Datasets)

## Start the notebook

```bash
cd ~/iot-dataset-lab
cp /path/to/dataset_starter.ipynb .
jupyter notebook dataset_starter.ipynb
```

## Exercises

### Exercise 1: Data Quality Audit (20 min)

Use AI to write code that answers:
- How many readings per sensor? Which died early?
- Missing values per sensor: random or clustered?
- Temperature distribution: what do the tails look like?
- Temperature over time for motes 1, 2, 15, 49

**Then write your interpretation:** Why did sensors die? What caused the outliers? When did Mote 49 stop?

### Exercise 2: Compare Sensors (20 min)

Use AI to write code that answers:
- Resample 4 sensors to 5-min averages and plot together
- Compute the correlation matrix
- Compute the max spread at any timestamp

**Then write your interpretation:** Why do some sensors diverge? Can you achieve ±0.5°C accuracy? How would you verify?

### Exercise 3: Anomaly Detection (15 min)

Use AI to build two detectors on Mote 1:
- Fixed thresholds: flag outside [15, 35]°C
- Rolling z-score: 1-hour window, |z| > 3

**Then write your interpretation:** Why do the methods find different anomalies? Which would you use for safety vs. drift detection?

## Deliverables

1. **LaTeX report** (PDF) — use `dataset_report_template.tex`, fill in all `YOUR_TEXT` placeholders
2. **Three plots** embedded in the report: `data_quality.png`, `sensor_comparison.png`, `anomalies.png`
3. **Jupyter notebook** (.ipynb) with your code
4. **Submit all on Canvas**

## Troubleshooting

| Problem | Fix |
|---------|-----|
| Dataset download fails | Use the GitHub mirror link above |
| Pandas can't parse the file | Use `sep=r'\s+'` and `na_values=['']` |
| Timestamps show NaT | Use `errors='coerce'` in `pd.to_datetime()` |
| Plots too slow / cluttered | Resample to 5-min or 15-min intervals |
| Rolling z-score gives NaN at start | Expected for the first hour; use `dropna()` |
