#!/usr/bin/env python3
"""
Load Tester per Robot Dialog Server
Simula multiple connessioni Furhat per testare il server
"""

import socket
import threading
import time
import random
import json
import argparse
from concurrent.futures import ThreadPoolExecutor
import statistics

class RobotDialogTester:
    def __init__(self, host='localhost', port=8081):
        self.host = host
        self.port = port
        self.results = []
        self.lock = threading.Lock()
        
    def generate_personality(self):
        """Genera personalità random per il test"""
        traits = {
            'extraversion': round(random.uniform(1.0, 7.0), 1),
            'agreeableness': round(random.uniform(1.0, 7.0), 1), 
            'conscientiousness': round(random.uniform(1.0, 7.0), 1),
            'emotional_stability': round(random.uniform(1.0, 7.0), 1),
            'openness': round(random.uniform(1.0, 7.0), 1)
        }
        return f"{traits['extraversion']},{traits['agreeableness']},{traits['conscientiousness']},{traits['emotional_stability']},{traits['openness']}"
    
    def generate_conversation(self):
        """Genera conversazione JSON per il test"""
        messages = [
            "Hello robot, how are you?",
            "Tell me a joke",
            "What's the weather like?",
            "How do you feel today?",
            "Can you help me with something?",
            "What do you think about AI?",
            "Tell me about yourself",
            "What are your capabilities?"
        ]
        
        message = random.choice(messages)
        conversation = [{
            "role": "user",
            "parts": [{"text": message}]
        }]
        return json.dumps(conversation)
    
    def single_request(self, client_id):
        """Esegue una singola richiesta al server"""
        start_time = time.time()
        result = {
            'client_id': client_id,
            'success': False,
            'response_time': 0,
            'error': None,
            'response_length': 0
        }
        
        try:
            # Connessione
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(30)
            sock.connect((self.host, self.port))
            
            # Ricevi welcome message
            welcome = sock.recv(1024).decode().strip()
            print(f"[Client {client_id}] Welcome: {welcome[:50]}...")
            
            # Genera richiesta
            personality = self.generate_personality()
            language = random.choice(['en', 'it', 'es', 'fr'])
            conversation = self.generate_conversation()
            
            request = f"1|extraversion:{personality}|{language}|{conversation}\n"
            print(request)
            # Invia richiesta
            sock.send(request.encode())
            
            # Ricevi risposta
            response = sock.recv(4096).decode().strip()
            
            # Calcola metriche
            end_time = time.time()
            result['success'] = True
            result['response_time'] = (end_time - start_time) * 1000  # ms
            result['response_length'] = len(response)
            
            print(f"[Client {client_id}] Response ({result['response_time']:.0f}ms): {response[:80]}...")
            
            sock.close()
            
        except Exception as e:
            result['error'] = str(e)
            print(f"[Client {client_id}] ERROR: {e}")
            
        with self.lock:
            self.results.append(result)
            
        return result
    
    def run_load_test(self, num_clients=10, max_workers=5):
        """Esegue il load test con N client"""
        print(f"Starting load test with {num_clients} clients, max {max_workers} concurrent")
        print(f"Target: {self.host}:{self.port}")
        print("-" * 60)
        
        start_time = time.time()
        
        # Esegui richieste con thread pool
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            futures = [executor.submit(self.single_request, i) for i in range(num_clients)]
                        
            # Aspetta completion
            for future in futures:
                future.result()
        
        total_time = time.time() - start_time
        self.print_statistics(total_time)
    
    def run_continuous_test(self, duration_seconds=60, concurrent_clients=5):
        """Esegue test continuo per X secondi"""
        print(f"Starting continuous test for {duration_seconds}s with {concurrent_clients} concurrent clients")
        print(f"Target: {self.host}:{self.port}")
        print("-" * 60)
        
        start_time = time.time()
        client_counter = 0
        
        def worker():
            nonlocal client_counter
            while time.time() - start_time < duration_seconds:
                client_counter += 1
                self.single_request(client_counter)
                time.sleep(random.uniform(0.5, 2.0))  # Pausa random tra richieste
        
        # Avvia worker threads
        threads = []
        for _ in range(concurrent_clients):
            t = threading.Thread(target=worker)
            t.start()
            threads.append(t)
        
        # Aspetta fine test
        for t in threads:
            t.join()
        
        total_time = time.time() - start_time
        self.print_statistics(total_time)
    
    def print_statistics(self, total_time):
        """Stampa statistiche del test"""
        if not self.results:
            print("No results to analyze")
            return
        
        successful = [r for r in self.results if r['success']]
        failed = [r for r in self.results if not r['success']]
        
        response_times = [r['response_time'] for r in successful]
        
        print("\n" + "=" * 60)
        print("LOAD TEST RESULTS")
        print("=" * 60)
        print(f"Total requests: {len(self.results)}")
        print(f"Successful: {len(successful)} ({len(successful)/len(self.results)*100:.1f}%)")
        print(f"Failed: {len(failed)} ({len(failed)/len(self.results)*100:.1f}%)")
        print(f"Total time: {total_time:.2f}s")
        print(f"Requests/second: {len(self.results)/total_time:.2f}")
        
        if response_times:
            print("\nResponse Time Statistics:")
            print(f"  Min: {min(response_times):.0f}ms")
            print(f"  Max: {max(response_times):.0f}ms")  
            print(f"  Avg: {statistics.mean(response_times):.0f}ms")
            print(f"  Median: {statistics.median(response_times):.0f}ms")
        
        if failed:
            print("\nError Summary:")
            error_counts = {}
            for r in failed:
                error = r['error'][:50] if r['error'] else 'Unknown'
                error_counts[error] = error_counts.get(error, 0) + 1
            
            for error, count in error_counts.items():
                print(f"  {error}: {count} times")

def main():
    parser = argparse.ArgumentParser(description='Load Tester for Robot Dialog Server')
    parser.add_argument('--host', default='localhost', help='Server host')
    parser.add_argument('--port', type=int, default=8081, help='Server port')
    parser.add_argument('--clients', type=int, default=10, help='Number of clients for burst test')
    parser.add_argument('--workers', type=int, default=5, help='Max concurrent workers')
    parser.add_argument('--continuous', type=int, help='Run continuous test for N seconds')
    parser.add_argument('--concurrent', type=int, default=3, help='Concurrent clients for continuous test')
    
    args = parser.parse_args()
    
    tester = RobotDialogTester(args.host, args.port)
    
    if args.continuous:
        tester.run_continuous_test(args.continuous, args.concurrent)
    else:
        tester.run_load_test(args.clients, args.workers)

if __name__ == '__main__':
    main()