from math import floor, ceil


"""
Metadata Classes
"""

class Metadata:

    def __init__(self, **kwargs):
        self.update(**kwargs)

    def update(self, **kwargs):
        pass


class AlarmMetadata(Metadata):

    def update(self, **kwargs):
        super().update(**kwargs)
        self.message = kwargs["message"]
        self.severity = kwargs.get("severity", None)
        self.watch_value_id = kwargs.get("watch_value_id", None)
        self.unit = kwargs.get("unit", None)
        if not self.severity:
            self.severity = 1

    @classmethod
    def empty(cls):
        return cls(message='')


class GraphicMetadata(Metadata):

    def update(self, **kwargs):
        super().update(**kwargs)
        self.name = kwargs["name"]
        self.x_min = kwargs["xmin"]
        self.x_max = kwargs["xmax"]
        self.y_min = kwargs["ymin"]
        self.y_max = kwargs["ymax"]
        self.x_unit = kwargs["xunit"]
        self.y_unit = kwargs["yunit"]
        self.x_name = kwargs.get("xname", "")
        self.y_name = kwargs.get("yname", "")

    @classmethod
    def empty(cls):
        return cls(name='', xmin=0, xmax=0, ymin=0, ymax=0, xunit='', yunit='')


class SensorMetadata(Metadata):

    def update(self, **kwargs):
        super().update(**kwargs)
        self.name = kwargs["name"]
        self.min = kwargs["minimum"]
        self.max = kwargs["maximum"]
        self.unit = kwargs["unit"]

    @classmethod
    def empty(cls):
        return cls(name='', minimum=0, maximum=0, unit='')


class ParameterMetadata(SensorMetadata):

    step_size = 0.01

    def update(self, step=None, default=None, **kwargs):
        super().update(**kwargs)
        self.step = step
        if not step:
            if not isinstance(self.min, str) and not isinstance(self.max, str):
                self.step = round((self.max - self.min) * self.step_size, 2)
            if isinstance(self.min, int) and isinstance(self.max, int):
                self.step = ceil(self.step)
        self.default = default
        if default is None or default == '':
            if not isinstance(self.min, str) and not isinstance(self.max, str):
                self.default = round((self.max + self.min)/2, 2)
            if isinstance(self.min, int) and isinstance(self.max, int):
                self.default = floor(self.default)


class BoundMetadata(AlarmMetadata, ParameterMetadata):

    def update(self, **kwargs):
        super().update(**kwargs) # Calls the update funciton from both parents
        self.bound_id = kwargs["bound_id"]

    @classmethod
    def empty(cls):
        return cls(name='', minimum=0, maximum=0, unit='', message='', severity=None, bound_id='')

