from time import time
from sensor import Sensor
from metadata import GraphicMetadata
from validation import CheckNothing
from base import DEBUG_GRAPHIC


class PartIterator:

    def __init__(self, *args):
        self.data = args
        self.length = min((len(d) for d in args))
        self.index = 0
    
    def __iter__(self):
        self.skip_nones()
        return self

    def __next__(self):
        while self.index < self.length:
            result = [d[self.index] for d in self.data]
            self.index += 1
            for item in result:
                if item == None:
                    raise StopIteration
            return result
        raise StopIteration

    def skip_nones(self):
        for d in self.data:
            while self.index < self.length and d[self.index] == None:
                self.index += 1


class Graphic(Sensor, metaclass=CheckNothing):

    id = "generic"
    NUM_POINTS = 320
    GAP_SIZE = 1

    def __init__(self):
        super().__init__()
        self.metadata = GraphicMetadata.empty()
        self.triggers = []
        self.snapshot = {}
        self.iterator = None
        self.update_frequency = 1
        self.relative_ticks = []
        self.insert_position = {}
        self.paused = True

    def activate(self, trigger, controller):
        super().activate(trigger, controller)
        if self.activated:
            if trigger not in self.snapshot:
                self.snapshot[trigger] = [None] * self.NUM_POINTS
                self.insert_position[trigger] = 0
            self.snapshot[trigger][self.insert_position[trigger]] = controller.data[trigger][-1]
            self.create_gap(trigger)

            if not self.paused:
                self.insert_position[trigger] = (self.insert_position[trigger] + 1) % self.NUM_POINTS

    def info(self):
        return {"name": self.metadata.y_name, "yrange": (self.metadata.y_min, self.metadata.y_max),
                "xrange": (self.metadata.x_min, self.metadata.x_max), "yunit": self.metadata.y_unit,
                "xunit": self.metadata.x_unit, "xname": self.metadata.x_name, "yname":self.metadata.y_name,
                "id": self.id}

    def create_gap(self, trigger):
        for i in range(1, self.GAP_SIZE + 1):
            target_index = (i + self.insert_position[trigger]) % self.NUM_POINTS
            self.snapshot[trigger][target_index] = None

    def extract(self, controller):
        super().extract(controller)
        timestamp = round(time())
        if not self.relative_ticks:
            reference = self.metadata.x_min
            step = (self.metadata.x_max - self.metadata.x_min) / self.NUM_POINTS
            self.relative_ticks = [reference + i * step for i in range(self.NUM_POINTS)]
        self.mount()
        result = []
        points = True
        while points:
            points = [{'x': x, 'y': y} for x, y in self.build()]
            if points:
                result.append(points)
        if DEBUG_GRAPHIC:
            print("LengthsGraphic[{}] = {}".format(self.id, sum([len(l) for l in result])))
        return timestamp, result

    def set_pause(self, pause):
        self.paused = pause

    def build(self):
        raise NotImplemented

    def mount(self):
        raise NotImplemented


class InspiratoryFlow(Graphic):

    id = "flow"

    def __init__(self):
        super().__init__()
        self.triggers.append("FIns")
        self.triggers.append("FExp")

    def mount(self):
        self.iterator = PartIterator(self.relative_ticks, self.snapshot["FIns"], self.snapshot["FExp"])

    def build(self):
        for tick, fins, fexp in self.iterator:
            if abs(fins - fexp) > 5:
           	 yield tick, fins - fexp
            else:
                 yield tick, 0


class InspiratoryPressure(Graphic):

    id = "pressure"

    def __init__(self):
        super().__init__()
        self.triggers.append("PIns1")

    def mount(self):
        self.iterator = PartIterator(self.relative_ticks, self.snapshot["PIns1"])

    def build(self):
        for tick, pins in self.iterator:
            yield tick, pins


class DeliveredVolume(Graphic):

    id = "volume"

    def __init__(self):
        super().__init__()
        self.triggers.append("mVTidal")

    def mount(self):
        self.iterator = PartIterator(self.relative_ticks, self.snapshot["mVTidal"])

    def build(self):
        for tick, vtidal in self.iterator:
            yield tick, vtidal * 1000


GRAPHICS = {graphic.id: graphic for graphic in Graphic.__subclasses__()}


if __name__ == '__main__':
    pass
