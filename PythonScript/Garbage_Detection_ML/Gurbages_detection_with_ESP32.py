import cv2
import numpy as np
import urllib.request
import os

url = 'http://192.168.109.93/cam-hi.jpg'

# Provide the absolute paths to the coco.names, yolov3.cfg, and yolov3.weights files
classesfile ='C:\\Users\\hp\\Desktop\\Projects\\Water-Pollution-Monitoring-and-Floating-Waste-Collection-Boat\\PythonScript\\Gurbage_Detection_ML\\coco.names'
modelConfig = 'C:\\Users\\hp\\Desktop\\Projects\\Water-Pollution-Monitoring-and-Floating-Waste-Collection-Boat\\PythonScript\\Gurbage_Detection_ML\\yolov3.cfg'
modelWeights = 'C:\\Users\\hp\\Desktop\\Projects\\Water-Pollution-Monitoring-and-Floating-Waste-Collection-Boat\\PythonScript\\Gurbage_Detection_ML\\yolov3.weights'

# Check if the paths are correct
print("COCO Names Path Exists:", os.path.exists(classesfile))
print("YOLO Config Path Exists:", os.path.exists(modelConfig))
print("YOLO Weights Path Exists:", os.path.exists(modelWeights))

# Read class names from coco.names
classNames = []
with open(classesfile, 'rt') as f:
    classNames = f.read().rstrip('\n').split('\n')

# Load YOLO model
net = cv2.dnn.readNetFromDarknet(modelConfig, modelWeights)
net.setPreferableBackend(cv2.dnn.DNN_BACKEND_OPENCV)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CPU)

confThreshold = 0.5
nmsThreshold = 0.3
whT = 320

def findObject(outputs, im):
    hT, wT, cT = im.shape
    bbox = []
    classIds = []
    confs = []
    found_cat = False
    found_bird = False
    for output in outputs:
        for det in output:
            scores = det[5:]
            classId = np.argmax(scores)
            confidence = scores[classId]
            if confidence > confThreshold:
                w, h = int(det[2] * wT), int(det[3] * hT)
                x, y = int((det[0] * wT) - w / 2), int((det[1] * hT) - h / 2)
                bbox.append([x, y, w, h])
                classIds.append(classId)
                confs.append(float(confidence))
    
    indices = cv2.dnn.NMSBoxes(bbox, confs, confThreshold, nmsThreshold)
    print(indices)
   
    if len(indices) > 0:
        for i in indices.flatten():
            box = bbox[i]
            x, y, w, h = box[0], box[1], box[2], box[3]
            if classNames[classIds[i]] == 'bird':
                found_bird = True
            elif classNames[classIds[i]] == 'cat':
                found_cat = True
            
            cv2.rectangle(im, (x, y), (x + w, y + h), (255, 0, 255), 2)
            cv2.putText(im, f'{classNames[classIds[i]].upper()} {int(confs[i] * 100)}%', (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 0, 255), 2)

while True:
    img_resp = urllib.request.urlopen(url)
    imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    im = cv2.imdecode(imgnp, -1)
    blob = cv2.dnn.blobFromImage(im, 1/255, (whT, whT), [0, 0, 0], 1, crop=False)
    net.setInput(blob)
    layernames = net.getLayerNames()
    outputNames = [layernames[i - 1] for i in net.getUnconnectedOutLayers()]

    outputs = net.forward(outputNames)

    findObject(outputs, im)

    cv2.imshow('Image', im)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
