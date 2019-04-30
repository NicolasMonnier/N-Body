import sys

from tempfile import TemporaryDirectory
from os.path import join, abspath

from qcg.appscheduler.service import QCGPMService
from qcg.appscheduler.tests.utils import save_reqs_to_file
from qcg.appscheduler.tests.job_stats import analyze_job_report

from shutil import copyfile


if 'profile' in globals():
    print('running with line_profiler')
else:
    print('running without line_profiler')


with TemporaryDirectory() as tmpdir:
    file_path = join(tmpdir, 'api-client-nbody.json')

    print('tmpdir: {}'.format(str(tmpdir)))

    jobName = "__api_client_${it}"
    nits = 20
    jobSleepTime = 1
    jobCores = 8
    availCores = 64
    rounds = nits * jobCores / availCores
    totalExecTime = rounds * jobSleepTime
    jobs = [
        {
            "name": jobName,
            "iterate": [0, nits],
            "execution": {
                "exec": "bash",
                "args": ["run_nbody.sh"],
#                "wd": abspath(join(tmpdir, jobName)),
                "stdout": "__api_client_${it}.stdout",
                "stderr": "__api_client_${it}.stderr"
            },
            "resources": {
                "numCores": {
                    "exact": jobCores,
                },
                "numNodes":{
                    "exact":1
                }
            }
        }
    ]
    reqs = [{'request': 'submit', 'jobs': jobs},
            {'request': 'control', 'command': 'finishAfterAllTasksDone'}]
    save_reqs_to_file(reqs, file_path)
    print('jobs saved to file_path: {}'.format(str(file_path)))

    print('starting scenario with {} jobs, anticipated execution time: {}'.format(nits, totalExecTime))

    sys.argv = ['python3 -m qcg.appscheduler.service', '--net', '--file', '--file-path', str(file_path), '--wd', str(tmpdir),
                '--report-format', 'json', '--log', 'error']
    QCGPMService().start()

    copyfile(join(tmpdir, 'jobs.report'), join('.', 'jobs.report.json'))
    copyfile(join(tmpdir, 'service.log'), join('.', 'service.log'))
    analyze_job_report(join('.', 'jobs.report.json'))
