import numpy as np
import pyaudio
import struct
from PyQt5.QtWidgets import QApplication
from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph.opengl as gl
import sys
from scipy.fftpack import fft
from matplotlib import cm
from opensimplex import OpenSimplex

class Terrain(object):
	def __init__(self):
		self.app = QApplication(sys.argv)
		self.window = gl.GLViewWidget()
		self.window.setGeometry(0, 110, 1920, 1080)
		self.window.show()
		self.window.setWindowTitle('TerrainMesh')
		self.window.setCameraPosition(distance=30, elevation=25)

		#grid = gl.GLGridItem()
		#grid.scale(2, 2, 2)
		#self.window.addItem(grid)

		self.nstep = 1
		self.ypoints = range(-20, 21 + self.nstep, self.nstep)
		self.xpoints = range(-20, 21 + self.nstep, self.nstep)
		#self.ypoints = range(-10, 12 + self.nstep, self.nstep)
		#self.xpoints = range(-10, 12 + self.nstep, self.nstep)
		
		self.nfaces = len(self.ypoints)

		self.RATE = 44100
		self.CHUNK = len(self.xpoints) * len(self.ypoints)

		self.p = pyaudio.PyAudio()
		self.stream = self.p.open(
			format=pyaudio.paInt16,
			channels=1,
			rate=self.RATE,
			input=True,
			output=True,
			frames_per_buffer=self.CHUNK)





		self.noise = OpenSimplex(seed=42)
		self.offset = 0

		verts = np.array([
				[
					x, y, 2 * self.noise.noise2(x=n / 5, y=m / 5)#np.random.normal(1)#0
				] for n, x in enumerate(self.xpoints) for m, y in enumerate(self.ypoints)
			], dtype=np.float32)

		faces = []
		colors = []
		for m in range(self.nfaces - 1):
			yoff = m * self.nfaces
			for n in range(self.nfaces - 1):
				faces.append([n + yoff, yoff + n + self.nfaces, yoff + n + self.nfaces + 1])
				faces.append([n + yoff, yoff + n + 1, yoff + n + self.nfaces + 1])
				colors.append([0, 0, 0, 0])
				colors.append([0, 0, 0, 0])

		faces = np.array(faces)
		colors = np.array(colors)
		self.mesh1 = gl.GLMeshItem(
			vertexes=verts,
			faces=faces,
			faceColors=colors,
			smooth=False,
			drawEdges=True
			)
		self.mesh1.setGLOptions('additive')
		self.window.addItem(self.mesh1)
		self.maxFreq = 0
		#self.minFreq = 999999


	def meshGen(self, offset=0, height=2, wf_data=None):
		
		if wf_data is not None:
			wf_data = struct.unpack(str(2 * self.CHUNK) + 'B', wf_data)
			wf_data = np.array(wf_data, dtype='b')[::2] + 128
			wf_data = np.array(wf_data, dtype='int32') - 128
			wf_data = wf_data / 50
			wf_data = wf_data.reshape((len(self.xpoints), len(self.ypoints)))
		
			freq_data = np.abs(fft(wf_data, axis=0)) #* 2
			freq_data = freq_data[:len(self.xpoints), :len(self.ypoints)]
		else:
			wf_data = np.ones(len(self.xpoints) * len(self.ypoints))
			wf_data = wf_data.reshape((len(self.xpoints), len(self.ypoints)))
			freq_data = np.ones_like(wf_data)




		verts = np.array([
				[
					x, y, 1.5 * wf_data[n][m] * self.noise.noise2(x=n / 5 + self.offset, y=m / 5 + self.offset)#np.random.normal(1)#0
				] for n, x in enumerate(self.xpoints) for m, y in enumerate(self.ypoints)
			], dtype=np.float32)

		faces = []
		colors = []
		colormap = cm.get_cmap("viridis")
		if np.max(freq_data) > self.maxFreq:
			self.maxFreq = np.max(freq_data)

		print(self.maxFreq)
		#color_value = np.mean(freq_data) / (self.maxFreq - self.minFreq) #* 2
		#color_value = np.log(1 + np.max(freq_data)) / np.log(1 + self.maxFreq)
		color_value = np.sqrt(np.mean(freq_data)) / np.sqrt(self.maxFreq) * 2
		color = colormap(color_value)[:3]
		for m in range(self.nfaces - 1):
			yoff = m * self.nfaces
			for n in range(self.nfaces - 1):
				faces.append([n + yoff, yoff + n + self.nfaces, yoff + n + self.nfaces + 1])
				faces.append([n + yoff, yoff + n + 1, yoff + n + self.nfaces + 1])
				#colors.append([n / self.nfaces, 1 - n/ self.nfaces, m/ self.nfaces, 0.6])
				#colors.append([n / self.nfaces, 1 - n/ self.nfaces, m/ self.nfaces, 0.75])

				#color_value = freq_data[n][m] / np.max(freq_data) * 2
				#color_value = freq_data[n][m] / (self.maxFreq - self.minFreq) * 2
				#color = colormap(color_value)[:3]
				colors.append([color[0], color[1], color[2], 0.6])
				colors.append([color[0], color[1], color[2], 0.75])

		faces = np.array(faces)
		colors = np.array(colors)
		return verts, faces, colors
	def update(self):

		wf_data = self.stream.read(self.CHUNK)
		verts, faces, colors = self.meshGen(self.offset, wf_data=wf_data)

		self.mesh1.setMeshData(
			vertexes=verts,
			faces = faces,
			faceColors=colors)

		self.window.opts["azimuth"] += 0.5
		self.offset -= 0.1



	def start(self):
		if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
			QApplication.instance().exec_()

	def animation(self):
		timer = QtCore.QTimer()
		timer.timeout.connect(self.update)
		timer.start(10)
		self.start()
		self.update()

if __name__=='__main__':
	t = Terrain()
	print(t)
	t.animation()
