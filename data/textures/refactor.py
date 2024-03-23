import json

data_filepath = 'tileset_old.json'
output_filepath = 'tileset.json'

file = open(data_filepath, 'r')
data = json.load(file)

def find_frame_by_id(game_id):
    for frame in data['frames']:
        if frame['game_id'] == game_id:
            return frame['frame']
    return -1

for frame in data['frames']:
    if 'angle' not in frame and 'front_face_id' not in frame:
        continue

    frame_id = frame['frame']
    front_face_frame = -1
    angle = ''

    if 'front_face_id' in frame:
        front_face_id = frame['front_face_id']
        front_face_frame = find_frame_by_id(front_face_id)

        if front_face_frame == -1:
            raise ValueError('Front face not found for frame ' + str(frame_id))

        del frame['front_face_id']

    if 'angle' in frame:
        angle = frame['angle']
        del frame['angle']

    faces = { }

    if front_face_frame != -1:
        frame['default_face'] = 'top'
        faces['top'] = frame_id
        faces['front'] = front_face_frame
    elif angle == 'orthogonal':
        frame['default_face'] = 'front'
        faces['front'] = frame_id
    else:
        frame['default_face'] = 'top'
        faces['top'] = frame_id

    frame['faces'] = faces

for frame in data['frames']:
    if 'faces' not in frame:
        continue

    del frame['frame']

with open(output_filepath, 'w') as output_file:
    json.dump(data, output_file, indent=4)
