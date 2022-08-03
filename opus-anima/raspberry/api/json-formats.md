# Expected JSON Formats

## list_modes():

**ENDPOINT**: '/modes', 'GET'

```json
{
	"modes":
	[
		{
			"label": <visual_string>,
			"id": <string_id>
		},
		{
			"label": <visual_string>,
			"id": <string_id>
		}
		...
	]

}
```

## retrieve_settings():

**ENDPOINT**: '/settings', 'GET'.

```json
{
	"modes":
	[
		{
			"mode": <mode_name>,
			"configuration": [
				{
					"name": <param_name>,
					"id" : <string>,
					"range": [<min>, <max>],
					"default": <value>,
					"step": <value>,
					"unit": <string>
				},
				{
					"name": <param_name>,
					"id" : <string>,
					"range": [<min>, <max>],
					"default": <value>,
					"step": <value>,
					"unit": <string>
				},
				...
			]
		},
		{
			"mode": <mode_name>,
			"configuration": [
				{
					"name": <param_name>,
					"id" : <string>,
					"range": [<min>, <max>],
					"default": <value>,
					"step": <value>,,
					"unit": <string>
				},
				{
					"name": <param_name>,
					"id" : <string>,
					"range": [<min>, <max>],
					"default": <value>,
					"step": <value>,
					"unit": <string>
				},
				...
			]
		},
		...
	],
	"output": 
	[
		{
			"name": <out_name>,
			"id": <out_id>,
			"range": [<min>, <max>],
			"default": <value>,
			"step": <value>,
			"unit": <string>
		},
		{	
			"name": <out_name>,
			"id": <out_id>,
			"range": [<min>, <max>],
			"default": <value>,
			"step": <value>,
			"unit": <string>
		},
		...
	],
	"graphics":
	[
		{
			"name": <g1_name>,
			"yrange": [<min>, <max>],
			"xrange": [<min>, <max>],
			"yunit": <string>,
			"xunit": <string>
		},
		{	
			"name": <g2_name>,
			"yrange": [<min>, <max>],
			"xrange": [<min>, <max>],
			"yunit": <string>,
			"xunit": <string>
		},
		...
	],
	"alarms":
	{
		"min":
		[
			{
				"name": <alarm_name>,
				"bound_id": <out_id>,
				"id" : <string>,
				"unit": <string>,
				"default": <value>,
				"step": <value>,
				"range": [<min>, <max>]
			},
			{
				"name": <out_name>,
				"bound_id": <out_id>,
				"id" : <string>,
				"unit": <string>,
				"default": <value>,
				"step": <value>,
				"range": [<min>, <max>]
			},
			...
		],
		"max":
		[
			{
				"name": <out_name>,
				"bound_id": <out_id>,
				"id" : <string>,
				"unit": <string>,
				"default": <value>,
				"step": <value>,
				"range": [<min>, <max>]
			},
			{
				"name": <out_name>,
				"bound_id": <out_id>,
				"id": <string>,
				"unit": <string>,
				"default": <value>,
				"step": <value>,
				"range": [<min>, <max>]
			},
			...
		]
	}
}
```

## configure():

**ENDPOINT**: '/configure', 'POST'.

```json
{
	"mode": <mode1_name>,
	"configuration": 
	{
		"param1_id": <value1>,
		"param2_id": <value2>,
		...
	}
}
```

## set_alarm_triggers():

**ENDPOINT**: '/boundaries', 'POST'

```json
{
	"boundaries":
	[
		"alarm1_id": <value>,
		"alarm2_id": <value>,
		...
	]
}

```

## report():

**ENDPOINT**: 'report', 'socketio.emit'.

```json
{
	"output": 
	[
		{
			"name": <out1_name>,
			"id": <string>,
			"value": <value1>,
			"range": [<min>, <max>]
		},
		{	
			"name": <out2_name>,
			"id": <string>,
			"value": <value2>,
			"range": [<min>, <max>]
		},
		...
	]
}
```

## report():

**ENDPOINT**: '/report', 'POST'.

```json
{
	"var1_name": <value1>,
	"var2_name": <value2>,
	...
}
```


## report():

**ENDPOINT**: 'draw', 'socketio.emit'.

```json
{
	"graphics": 
	[
		{
			"name": <g1_name>,
			"timestamp": <value>,
			"value": 
			[
				{
					'x': <value1>, 
					'y': <value2>
				}, 
			...
			] 
		},
		{	
			"name": <g2_name>,
			"timestamp": <value>,
			"value": 
			[
				{
					'x': <value1>, 
					'y': <value2>
				}, 
			...
			] 
		},
		{	
			"name": <g3_name>,
			"timestamp": <value>,
			"value": 
			[
				{
					'x': <value1>, 
					'y': <value2>
				}, 
			...
			] 
		},
		...
	]
}
```

## report():

**ENDPOINT**: 'alert', 'socketio.emit'.

```json
{
	"alerts":
	[
		{
			"id": <string>,
			"bound_id": <out_id>,
			"message": <string>,
			"severity": <integer>,
			"value": <float>
		},
		...
	]
}
```

## operator_actions():

**ENDPOINT**: '/silence', 'POST'.

```json
{
	"alarm": <alarm_id>
	"value": <integer>
}
```

## add_patient()

**ENDPOINT**: '/patient', 'POST'

```json
{
	"name": <string>,
	"age_group": <string>,
	"weight": <integer>,
	"height": <integer>,
	"prontuary": <string>,
	"gender": <string>
}
```