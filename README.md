# Data averaging tool

This tool computes averaged data from discrete time point records, including interpolation of missing data.

# Example

For the following data, first colum is time column, second column is data column:

	1	100
	3	50
	4	1
	5	50
	6	20
	9	90
	10	10
	11	20
	14	80
	15	30

The interpolated intermediate data would be:

	1	100
	2	75
	3	50
	4	1
	5	50
	6	20
	7	43.33
	8	66.66
	9	90
	10	10
	11	20
	12	40
	13	60
	14	80
	15	30

And 5-element running averages, each 3-elemnts would be:

3	65.2
5	32.8667
8	46
11	44
14	48

To get the following output, execute: `averager --average-frequency 3 --average-span 5 -i input -o output`
